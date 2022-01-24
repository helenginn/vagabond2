#include "glm_import.h"

glm::mat3x3 mat3x3_rhbasis(glm::vec3 a, glm::vec3 b);

template <class T>
inline bool is_glm_vec_sane(T vec)
{
	for (size_t i = 0; i < vec.length(); i++)
	{
		if (vec[i] != vec[i] || !isfinite(vec[i]))
		{
			return false;
		}
	}
	
	return true;
}

/** matrix from three bond lengths and three bond angles
 * @param a unit length
 * @param b unit length
 * @param c unit length
 * @param alpha in degrees
 * @param beta in degrees
 * @param gamma in degrees
 * @returns matrix where vectors point in directions obeying supplied restraints
 * */
glm::mat3x3 mat3x3_from_unit_cell(double a, double b, double c, 
                                  double alpha, double beta, double gamma);

glm::mat4x4 torsion_basis(glm::mat4x4 prior, glm::vec3 prev, glm::vec4 next);
