#include "GraphicsDebugUtils.hpp"
#include "Lights.hpp"
#include "Material.hpp"

ostream& operator<<(ostream& os, const DirectionalLight& l)
{
    using namespace std;
    os << endl;
    os << "{" << endl;
    os << "amb: " << l.ambient << endl;
    os << "dif: " << l.diffuse << endl;
    os << "spe: " << l.specular << endl;
    os << "dir: " << l.direction << endl;
    os << "}" << endl;
    return os;
}

ostream& operator<<(ostream& os, const Material& m)
{
    using namespace std;
    os << endl;
    os << "{" << endl;
    os << "amb: " << m.ambient << endl;
    os << "dif: " << m.diffuse << endl;
    os << "spe: " << m.specular << endl;
    os << "ref: " << m.reflect << endl;
    os << "}" << endl;
    return os;
}
