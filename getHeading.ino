void getHeading() {
  
//Donne la direction cardinale du Vent

  if (calDirection < 22.5)
    directionLettre = "N ";
  else if (calDirection < 67.5)
    directionLettre = "NE";
  else if (calDirection < 112.5)
    directionLettre = "E ";
  else if (calDirection < 157.5)
    directionLettre = "SE";
  else if (calDirection < 202.5)
    directionLettre = "S ";
  else if (calDirection < 247.5)
    directionLettre = "SW";
  else if (calDirection < 292.5)
    directionLettre = "W ";
  else if (calDirection < 337.5)
    directionLettre = "NW";
  else
    directionLettre = "N ";
}
