void getWindDirection() {

//Donne la direction du Vent en °



  //vaneDirection = map(vaneValue*10, 0.6*10, 4.4*10, 0, 360);
  // Utiliser les fractions résoud le problème de map qui n'utilise que des entiers
  //Dans vaneDirection = (vaneValue *(360-0)/ (X-Y));
  // X = voltage le plus fort
  // Y = voltage le plus fort
  vaneDirection = (vaneValue *(360-0)/ (4.38-0.06));
  
  calDirection = vaneDirection + VaneOffset;

    if (calDirection > 360)
    calDirection = calDirection - 360;

  if (calDirection < 0)
    calDirection = calDirection + 360;
  
  calDirection = vaneDirection + VaneOffset;
}
