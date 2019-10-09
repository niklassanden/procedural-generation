#pragma once

#include "Tables.h"

#include "glm/glm.hpp"

namespace ProceduralGeneration {
	class Noise {
		// ----------------------------------------------------------------------------
		// Improved perlin noise
		// ----------------------------------------------------------------------------
	public:
		static float octavePerlin(float x, float y, float z, int octaves, float persistence) {
			x = glm::abs(x + 100000.0f);
			y = glm::abs(y + 100000.0f);
			z = glm::abs(z + 100000.0f);

			float total = 0.0f;
			float frequency = 0.2f;
			float amplitude = 1.0f;
			float maxValue = 0.0f;			// Used for normalizing result to 0.0 - 1.0
			for (int i = 0; i < octaves; i++) {
				total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;

				maxValue += amplitude;

				amplitude *= persistence;
				frequency *= 2.0f;
			}

			return total / maxValue;
		}

	private:
		static float perlin(float x, float y, float z) {
			int xi = (int)x & 255;								// Calculate the unit cube that the point asked will be located in
			int yi = (int)y & 255;								// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
			int zi = (int)z & 255;								// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
			float xf = x - (int)x;								// We also fade the location to smooth the result.
			float yf = y - (int)y;
			float zf = z - (int)z;
			float u = fade(xf);
			float v = fade(yf);
			float w = fade(zf);

			int aaa, aba, aab, abb, baa, bba, bab, bbb;
			aaa = Tables::permutations[Tables::permutations[Tables::permutations[xi    ] + yi    ] + zi    ];
			aba = Tables::permutations[Tables::permutations[Tables::permutations[xi    ] + yi + 1] + zi    ];
			aab = Tables::permutations[Tables::permutations[Tables::permutations[xi    ] + yi    ] + zi + 1];
			abb = Tables::permutations[Tables::permutations[Tables::permutations[xi    ] + yi + 1] + zi + 1];
			baa = Tables::permutations[Tables::permutations[Tables::permutations[xi + 1] + yi    ] + zi    ];
			bba = Tables::permutations[Tables::permutations[Tables::permutations[xi + 1] + yi + 1] + zi    ];
			bab = Tables::permutations[Tables::permutations[Tables::permutations[xi + 1] + yi    ] + zi + 1];
			bbb = Tables::permutations[Tables::permutations[Tables::permutations[xi + 1] + yi + 1] + zi + 1];

			float x1, x2, y1, y2;
			x1 = lerp(grad(aaa, xf, yf, zf),				// The gradient function calculates the dot product between a pseudorandom
				grad(baa, xf - 1.0f, yf, zf),				// gradient vector and the vector from the input coordinate to the 8
				u);										// surrounding points in its unit cube.
			x2 = lerp(grad(aba, xf, yf - 1.0f, zf),				// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
				grad(bba, xf - 1.0f, yf - 1.0f, zf),				// values we made earlier.
				u);
			y1 = lerp(x1, x2, v);

			x1 = lerp(grad(aab, xf, yf, zf - 1.0f),
				grad(bab, xf - 1.0f, yf, zf - 1.0f),
				u);
			x2 = lerp(grad(abb, xf, yf - 1.0f, zf - 1.0f),
				grad(bbb, xf - 1.0f, yf - 1.0f, zf - 1.0f),
				u);
			y2 = lerp(x1, x2, v);

			return (lerp(y1, y2, w) + 1.0f) / 2.0f;						// For convenience we bound it to 0 - 1 (theoretical min/max before is -1 - 1)
		}

		static float grad(int hash, float x, float y, float z) {
			int h = hash & 15;									// Take the hashed value and take the first 4 bits of it (15 == 0b1111)
			float u = h < 8 /* 0b1000 */ ? x : y;				// If the most significant bit (MSB) of the hash is 0 then set u = x.  Otherwise y.

			float v;											// In Ken Perlin's original implementation this was another conditional operator (?:).  I
																// expanded it for readability.

			if (h < 4 /* 0b0100 */)								// If the first and second significant bits are 0 set v = y
				v = y;
			else if (h == 12 /* 0b1100 */ || h == 14 /* 0b1110*/)// If the first and second significant bits are 1 set v = x
				v = x;
			else 												// If the first and second significant bits are not equal (0/1, 1/0) set v = z
				v = z;

			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v); // Use the last 2 bits to decide if u and v are positive or negative.  Then return their addition.
		}

		static float fade(float t) {
			// Fade function as defined by Ken Perlin.  This eases coordinate values
			// so that they will ease towards integral values.  This ends up smoothing
			// the final output.
			return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);			// 6t^5 - 15t^4 + 10t^3
		}

		static float lerp(float a, float b, float x) {
			return a + x * (b - a);
		}
	};
}