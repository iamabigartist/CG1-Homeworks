#include "Utility.hpp"
#include <algorithm>

/**
 * generate a uniform_clamped knots vector
 * @param[out] knots the knots vector
 * @param[in] m the number of controll point
 * @param[in] p the degree of the dimension
 */
void uniform_clamped_knots_vector(vector<float>& knots, int m, int p)
{
	if (m - p <= 0)throw "The controll points number must be more than the degree!";

	//the number of knots
	int h = m + p + 1;
	float step = 1.0 / (m - p);
	for (int k = 0; k < h; k++)
	{
		if (k < p)
		{
			knots.push_back(0);
		}
		else if (k > h - p - 1)
		{
			knots.push_back(1);
		}
		else//p <= k <= h-p-1
		{
			knots.push_back((k - p) * step);
		}
	}
	return;
}

/**
 * generate a uniform_open knots vector
 * @param[out] knots the knots vector
 * @param[in] m the number of controll point
 * @param[in] p the degree of the dimension
 */
void uniform_open_knots_vector(vector<float>& knots, int m, int p)
{
	if (m - p <= 0)throw "The controll points number must be more than the degree!";

	//the number of knots
	int h = m + p + 1;
	float step = 1.0 / (h - 1);
	for (int k = 0; k < h; k++)
	{
		knots.push_back(k * step);
	}
	return;
}

/// <summary>
/// Judge if u is in range of the curve and get the index of u_k
/// </summary>
/// <param name="k">The index of u_k that is less equal than u </param>
/// <param name="p">The degree</param>
/// <param name="u">The given knot value</param>
/// <param name="knots">The knots vector</param>
/// <returns>Whether the knot value is in the range of the curve</returns>
bool in_curve_range(int& k, const int& p, const float& u, const vector<float>& knots)
{
	//u can't be larger than 1, the last knot is 1.
	//and can't be smaller than 0, the first knot is 0.

	//u_(k+1) is the first knot that is larger than u.
	auto u_k_1 = upper_bound(knots.begin(), knots.end(), u);

	k = std::distance(knots.begin(), u_k_1) - 1;

	//if k < p or k > tail-p, then don't draw the point
	if (k < p || knots.size() - 1 - p - 1 < k)
	{
		return false;
	}

	return true;
}