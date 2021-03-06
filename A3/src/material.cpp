#include "material.hpp"

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
  // Perform OpenGL calls necessary to set up this material.
    glShadeModel ( GL_SMOOTH );
    glEnable ( GL_LIGHTING );
    glEnable ( GL_LIGHT1 );
}

Colour PhongMaterial::getColor() const
{
    return m_kd;
}
