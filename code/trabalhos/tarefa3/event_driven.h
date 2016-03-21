/* Funcoes de registro */
void button_listen(int pin);
void timer_set(int ms);

/* Callbacks */
void button_changed(int pin, int value);
void timer_expired();

/* Init */
void my_init();
