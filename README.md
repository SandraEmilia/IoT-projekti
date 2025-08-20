# IoT-projekti

Projektissa on luotu IoT-laite käyttämällä Arduino NANO 33 IoT -laitetta. 
Laitteeseen on yhdistetty DHT11 lämpöanturi, OlED-näyttö sekä servomoottori. Lämpöanturi mittaa huoneen lämpötilaa ja tulostaa sen näytölle. 

Laitteen käytölle on luotu taustapalvelin JSON:lla ja selaimella käyttäjä voi käydä asettamassa laitteelle uuden min/max lämpötilan. Jos lämpötila laskee min-lämpötilan alapuolelle, laite ilmoittaa olevan liian kylmä ja sulkee servomoottorin luukun, jos lämpötila on yli max-lämmön, laite ilmoittaa lämpötilan olevan liian kuuma ja avaa servomoottorin luukun.

