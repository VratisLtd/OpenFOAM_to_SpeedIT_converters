#ifndef __OF2SPEEDIT_DIVSCHEMES_H__
#define __OF2SPEEDIT_DIVSCHEMES_H__

#include "fvCFD.H"
#include <fstream>

template<class T>
int divSchemeSetting(std::ostream & f, Foam::word whichDiv, T & name, Foam::fvMesh & mesh)
{
    word divI = mesh.divScheme(whichDiv)[0].wordToken();
    word divT = mesh.divScheme(whichDiv)[1].wordToken();

    if (divI == "Gauss" && (divT == "linear" || divT == "upwind"))
    {
        setting_name(f, name) << divI << divT << "\n";
    }
    else
        throw std::string("Unsupported divScheme ") + divI + " " + divT +
              ", the only supported schemes are Gauss linear and Gauss upwind";

    return 0;
}

#endif // __OF2SPEEDIT_DIVSCHEMES_H__

