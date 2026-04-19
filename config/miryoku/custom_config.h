// Copyright 2021 Manna Harbour
// https://github.com/manna-harbour/miryoku

#define MIRYOKU_ALPHAS_QWERTY
#define MIRYOKU_EXTRA_COLEMAKDH
#define MIRYOKU_NAV_VI

// Put Studio Unlock on the Button layer, first key
#define MIRYOKU_LAYER_BUTTON \
&studio_unlock,    &kp MCLK,          &kp RCLK,          &kp LCLK,          &trans,            &trans,            &kp LCLK,          &kp RCLK,          &kp MCLK,          &trans,            \
&kp LGUI,          &kp LALT,          &kp LCTRL,         &kp LSHFT,         &trans,            &trans,            &kp LSHFT,         &kp LCTRL,         &kp LALT,          &kp LGUI,          \
&trans,            &kp ALGR,          &trans,            &trans,            &trans,            &trans,            &trans,            &trans,            &kp ALGR,          &trans,            \
&kp LCLK,          &kp RCLK,          &kp MCLK,          &kp MCLK,          &kp RCLK,          &kp LCLK
