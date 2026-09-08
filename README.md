# CEP
Code
Mes test pour verifer INPUT/OUTPUT: 

        OUTPUT       INPUT        INPUT attendu
Test 1  PE2          PE4          0 puis 1
Test 2  PE4          PE2          0 puis 1
Test 3  PE3          PE5          0 puis 1
Test 4  PE5          PE3          0 puis 1


Mes tests pour verifier Short-circuit:  
     
        PIN          ETAT        INPUT attendu
Test 5  PE2          HIGH          PE4 lit HIGH et PE3/PE5 ne lisent rien ou reste à leur état par défaut
Test 6  PE2          LOW           PE4 lit LOW et PE3/PE5 ne lisent rien ou reste à leur état par défaut
Test 7  PE3          HIGH          PE5 lit HIGH et PE2/PE4 ne lisent rien ou reste à leur état par défaut
Test 8  PE3          LOW           PE5 lit LOW et PE2/PE4 ne lisent rien ou reste à leur état par défaut
