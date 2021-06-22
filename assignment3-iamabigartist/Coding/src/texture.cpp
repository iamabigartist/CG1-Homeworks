#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "utils.hpp"

using namespace Eigen;

Texture::Texture ()
	: data ( nullptr )
	, width ( 0 )
	, height ( 0 )
{
}

bool Texture::isNull () const
{
	return data == nullptr || width == 0 || height == 0;
}

Vector2i Texture::getDimension () const
{
	return Vector2i ( width, height );
}

Vector3f Texture::Sample ( Vector2f uv ) const
{
#define REPEAT_MODE

	//获取一个像素的nv大小 OK
	//wrap uv OK
	//取uv 附近四个像素getPixel
	//根据到左上角像素的横竖距离与一个像素uv大小比, 加权平均四个像素颜色
	//返回这个颜色

#pragma region Set the wrap mode

	typedef Vector2f ( Texture::* wrap )( Vector2f )const;

#ifdef CLAMP_MODE
	wrap uv_wrap = &Texture::clamp_uv;
	wrap pos_wrap = &Texture::clamp_pos;
#endif // CLAMP_MODE

#ifdef REPEAT_MODE
	wrap uv_wrap = &Texture::repeat_uv;
	wrap pos_wrap = &Texture::repeat_pos;
#endif // REPEAT_MODE

#pragma endregion

	uv = ( this->*uv_wrap )( uv );
	Vector2f pos = uv_to_pos ( uv );

	//texture coordinate position
	Vector2f pos_00 = ( this->*pos_wrap )( Vector2f ( floor ( pos.x () ), floor ( pos.y () ) ) );
	Vector2f pos_11 = ( this->*pos_wrap )( Vector2f ( ceil ( pos.x () ), ceil ( pos.y () ) ) );
	Vector2f pos_01 = ( this->*pos_wrap )( Vector2f ( floor ( pos.x () ), ceil ( pos.y () ) ) );
	Vector2f pos_10 = ( this->*pos_wrap )( Vector2f ( ceil ( pos.x () ), floor ( pos.y () ) ) );
	float ratio_u = pos.x () - pos_00.x ();
	float ratio_v = pos.y () - pos_00.y ();
	return
		( ratio_u ) * ( ratio_v ) *getPixel ( pos_11 ) +
		( 1 - ratio_u ) * ( 1 - ratio_v ) * getPixel ( pos_00 ) +
		( 1 - ratio_u ) * ratio_v * getPixel ( pos_01 ) +
		( ratio_u ) * ( 1 - ratio_v ) * getPixel ( pos_10 );
	//return getPixel ( pos_00 );
}

void Texture::clear ()
{
	width = 0;
	height = 0;
	delete data;
	data = nullptr;
}

Texture::~Texture ()
{
	clear ();
}

Texture Texture::loadFromPath ( std::string path )
{
	int w, h, n;
	unsigned char* raw_data = stbi_load ( path.c_str (), &w, &h, &n, 0 );

	Texture tex;

	/* Only images with 1 or 3 channels are supported */
	if ( raw_data && ( n == 3 || n == 1 ) )
	{
		tex.data = new float [ w * h * 3 ];
		tex.width = w;
		tex.height = h;

		int ch_stride = n == 3 ? 1 : 0;
		for ( int i = 0; i < w * h * n; i += n )
		{
			tex.data [ i ] = ( float ) raw_data [ i ] / 255.0f;
			tex.data [ i + 1 ] = ( float ) raw_data [ i + ch_stride ] / 255.0f;
			tex.data [ i + 2 ] = ( float ) raw_data [ i + ch_stride * 2 ] / 255.0f;
		}

		stbi_image_free ( raw_data );
	}

	return tex;
}

void Texture::generateCheckerboard ( Texture& tex, Vector3f primary_rgb, Vector3f secondary_rgb, unsigned int num_ckers )
{
	int checker_size = 32;
	int w = num_ckers * checker_size, h = num_ckers * checker_size;

	Vector3f color_list [ 2 ];
	color_list [ 0 ] = primary_rgb;
	color_list [ 1 ] = secondary_rgb;

	tex.width = w;
	tex.height = h;
	tex.data = new float [ tex.width * tex.height * 3 ];

	for ( int y = 0; y < h; ++y )
	{
		for ( int x = 0; x < w; ++x )
		{
			Vector3f color = color_list [ ( ( int ) ( x / checker_size ) + ( int ) ( y / checker_size ) ) % 2 ];
			tex.data [ ( y * w + x ) * 3 ] = color [ 0 ];
			tex.data [ ( y * w + x ) * 3 + 1 ] = color [ 1 ];
			tex.data [ ( y * w + x ) * 3 + 2 ] = color [ 2 ];
		}
	}
}