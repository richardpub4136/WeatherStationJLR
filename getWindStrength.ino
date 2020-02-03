void getWindStrength(float windspeed_kmh) {

  if (windspeed_kmh <= 1)
    forceVent = "Calme";
  else if (windspeed_kmh = 2 && windspeed_kmh <= 5) 
    forceVent = "Tres Legere Brise";
   else if (windspeed_kmh = 6 && windspeed_kmh <= 12) 
    forceVent = "Legere Brise";
  else if (windspeed_kmh = 13 && windspeed_kmh <= 19) 
    forceVent = "Petite brise"; 
  else if (windspeed_kmh = 20 && windspeed_kmh <= 28)
    forceVent = "Jolie Brise"; 
  else if (windspeed_kmh = 29 && windspeed_kmh <= 38)
    forceVent = "Bonne Brise"; 
  else if (windspeed_kmh = 39 && windspeed_kmh <= 50)
    forceVent = "Vent Frais"; 
  else if (windspeed_kmh = 51 && windspeed_kmh <= 61)
    forceVent = "Grand Frais"; 
  else if (windspeed_kmh = 62 && windspeed_kmh <= 74)
    forceVent = "Coup de Vent"; 
  else if (windspeed_kmh = 75 && windspeed_kmh <= 88)
    forceVent = "Fort Coup de Vent"; 
  else if (windspeed_kmh = 75 && windspeed_kmh <= 88)
    forceVent = "Fort Coup de Vent"; 
  else if (windspeed_kmh = 89 && windspeed_kmh <= 102)
    forceVent = "Tempete"; 
  else if (windspeed_kmh = 103 && windspeed_kmh <= 117)
    forceVent = "Violente Tempete";  
  else if (windspeed_kmh = 118 && windspeed_kmh <= 132)
    forceVent = "Ouragan";  
  else if ( windspeed_kmh > 133)
    forceVent = "Planquez Vous !!!"; 
  else
    forceVent = "Trop Tard !!!"; 
}
