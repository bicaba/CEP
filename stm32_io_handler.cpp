// Implement firmware to handle STM32 GPIO's
// It is recommanded to write inside User Code areas, but that's not a hard
// constraint. Do what's best.

#include <cstdint>

/** Start User Code */
// od.io contient les 1,2,3,4 qui représentes les pin PE2,PE3,PE4 et PE5. cette fonction traduit en
// pin pour la stm32. elle retourne 0 si io n'est pas compris entre 1 et 4
// 1 devient GPIO_PIN_2..  16 bits pour les pin de stm32
uint16_t PinFromIo(uint8_t io) {
  switch (io) {
    case 1: return GPIO_PIN_2;  // si io=2 on a PE2
    case 2: return GPIO_PIN_3;  // si io=3 on a PE3
    case 3: return GPIO_PIN_4;  // PE4
    case 4: return GPIO_PIN_5;  // PE5
    default: return 0;          // SI io different de 1....4 on ignore
  }
}
/** End User Code */

enum class Command : uint8_t {
  NONE = 0,
  SET_CONFIG,
  GET_STATE,
  SET_STATE,
};

struct OD {
  uint8_t command = 0;
  uint8_t io = 0; // me donne le numéro des broches (PE2,PE3,PE5,PE4)
  uint8_t mode = 0; // mode Input ou Output
  uint8_t pull = 0; // resistence interne (aucune, pull-up, pull-down)
  uint8_t state = 0; // High ou Low pour l'état des broches
};

extern OD od;  // Owned by CANOPen module

/** Start User Code */
// cette fonction permet de configurer la broche en disant au MCU
// si la broche doit être en entré ou sortie et sa résistence interne

void HandleConfig() {
  uint16_t pin = PinFromIo(od.io); // indique quel broche on configure
  if (pin == 0)  {
    return;  // broche inconnue  on ne fait rien
    }

  GPIO_InitTypeDef init{};
  init.Pin = pin;
  init.Speed = GPIO_SPEED_FREQ_LOW; // frequence low pour economiser énergie

  //  Si od.mode est égale à 2  alors on est en mode OUTPUT sinon en mode INPUT
  if (od.mode == 2){
    init.Mode = GPIO_MODE_OUTPUT_PP;
    }
  else {
    init.Mode = GPIO_MODE_INPUT;
    }

  // od.pull : 1 = NONE, 2 = PULLUP, 3 = PULLDOWN
  if (od.pull == 2) {
    init.Pull = GPIO_PULLUP;
    }
  else if (od.pull == 3) {
    init.Pull = GPIO_PULLDOWN;
    }
  else {
    init.Pull = GPIO_NOPULL;
    }

  // Petit détail important : si on configure en sortie, on écrit d'abord
  // la valeur voulue AVANT d'appliquer la config, pour ne pas envoyer
  // un état "au hasard" pendant une fraction de seconde.
  if (init.Mode == GPIO_MODE_OUTPUT_PP) {
    if (od.state == 1)
      HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_RESET); 
  }

  HAL_GPIO_Init(GPIOE, &init);  // applique vraiment la config
}
// maintenant on veur lire l'état de la broche pour savoir si elle 0 ou 1 puis
// on enregistre la réponses dans od.state pour que frasy puisse la voir
void HandleGetState() {
  uint16_t pin = PinFromIo(od.io);
  if (pin == 0) return;

  // HAL_GPIO_ReadPin renvoie GPIO_PIN_SET (haut) ou GPIO_PIN_RESET (bas)
  if (HAL_GPIO_ReadPin(GPIOE, pin) == GPIO_PIN_SET)
    od.state = 1;
  else
    od.state = 0;;


}
void HandleSetState() {
  uint16_t pin = PinFromIo(od.io);
  if (pin == 0)
    return;

  if (od.state == 1)
    HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(GPIOE, pin, GPIO_PIN_RESET);
}

/** End User Code */

namespace IO {
void handle() {
  // This function is periodically invoked from main.
  // od object values will change when receiving commands from Frasy.
  // You must read od variables and serve Frasy requests.

  if (od.command == 0)
    return;  // od.command is 0, Frasy has yet to request us to do something.

  // od.command is not 0! Frasy asked us to do something.
  // Implement the code to handle the following commands.
  // * Set target GPIO configuration
  // * Read target GPIO state
  // * Set target GPIO state

  /** Start User Code */
  switch (od.command) {
    case 1: HandleConfig(); break;  // SET_CONFIG
    case 2: HandleGetState();  break;  // GET_STATE
    case 3: HandleSetState();  break;  // SET_STATE
    default: break;
  }

  /** End User Code */

  od.command = 0;  // we handled the command, set od.command back to 0 to tell
                   // frasy we're done
}
}  // namespace IO