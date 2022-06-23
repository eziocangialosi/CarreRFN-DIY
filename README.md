# CarreRFN-DIY

Reproduction of an french railway signal at the 1/2 scale

# French Section (English Below) :
## Objectif :
Reproduction d'un signal typique du réseau ferroviaire français à but décoratif
et à l'échelle 1/2.
Plus précisément, un "carré" à 5 feux (dont 1 bouché) plus un oeuilleton.
Le signal sera commandée à partir d'un ESP8266 et à l'aide d'une interface web, le tout
communiquant grâce au protocole MQTT.

![Photo d'un "carré" sur une potence](https://github.com/b84500/CarreRFN-DIY/blob/main/img/carrePot.jpg)

## Mise en plan du projet :
La première phase du projet à été sa mise en plan et modélisation 3D sous SolidWorks.

![Screenshot de la modélisation sous Soldiworks](https://github.com/b84500/CarreRFN-DIY/blob/main/img/CarreSLDW.PNG)

Cette mise en plan à été faite partir de documents et plans trouvés sur internet
(rassemblés dans le dossier data)

## Réalisation :
### Cible et Oeuilleton :
La Cible et l'Oeuilleton ont été réalisés en bois puis peints en noir mat avec une bombe à peinture.

![Photo de l'Oeuilleton et de la Cible](https://github.com/b84500/CarreRFN-DIY/blob/main/img/carre&oeuilt.png)

### Casquettes, diffuseurs et fonds de feux
Les casquettes qui protègent les feux ont étés réalisées grâce à l'impression 3D en PLA.
Il en est de même pour les fonds de feux. Les diffuseurs vont probalement être réalisés
également via l'impression 3D mais en PETG transparent.

![Photo des casquettes et fond de feu de l'oeuilleton](https://github.com/b84500/CarreRFN-DIY/blob/main/img/casquettes+fdfOeuilt.png)

### Lumière et partie éléctronique
L'électronique du projet s'articule autour d'un ESP8266. Du strip led adressable (WS2811) va
être utilisé faire l'allumage des feux.

### Décorations
Le principal point de décoration sur le projet, est le contour blanc de la cible. La solution
la plus adéquate me semble être la découpe de vinyle autocollant.
