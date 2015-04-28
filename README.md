# Beeldverwerking

Er is dus unaniem beslist om met OpenCV 3.0 (beta) te werken.

--

Kleine opmerking bij de RD-files van Frederick:
- de constante CV_WINDOW_AUTOSIZE is bij mij gewoon WINDOW_AUTOSIZE
- de methode cvCreateTrackbar is bij mij createTrackbar
- ik moet de include <cv.h> vervangen door <opencv/cv.h>
Gebruik jij ook OpenCV 3.0 Frederick?
'k Heb sowieso alles van jou in commentaar erin gelaten.
't Kan dus wel zijn dat je deze wijzigingen weer zal moeten terugdraaien
om het bij u te kunnen runnen.


Voor de rest krijg ik een error bij linehandler.h dat een bepaalde constante
dubbel gedefinieerd is, eens in OpenCV en eens in Windows zelf (wss door Win8).
Ik kan het oplossen door 'using namespace cv;' weg te doen
en alles te laten voorafgaan door 'cv::'.
Dit maakt voor de rest dus niets uit kwa functionaliteit.

      -Jens
--
