// Copyright 2021 Manna Harbour
// https://github.com/manna-harbour/miryoku

#define MIRYOKU_ALPHAS_QWERTY
#define MIRYOKU_EXTRA_COLEMAKDH
#define MIRYOKU_NAV_VI

// We manually pad the 36-key layout to 40 keys using &none
#define MIRYOKU_LAYER_BUTTON \
&studio_unlock, &studio_unlock, U_BTN3, U_BTN2, U_BTN1, U_BTN1, U_BTN2, U_BTN3, &trans, &none, \
&none, &kp LGUI, &kp LALT, &kp LCTRL, &kp LSHFT, &kp LSHFT, &kp LCTRL, &kp LALT, &kp LGUI, &none, \
&none, &trans, &kp ALGR, &trans, &trans, &trans, &trans, &kp ALGR, &trans, &none, \
&none, &none, U_BTN1, U_BTN2, U_BTN3, U_BTN3, U_BTN2, U_BTN1, &none, &none
