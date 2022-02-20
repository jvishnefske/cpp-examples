h_value(uint8_t data);
bool wait_until_ready_to_send(void)

// take a single byte and send it to push_value when ready.
void send_byte(uint8_t data);
// take a single byte and send it to push_value when ready.

// take an array, and send each one of them to push_value when ready.
void send_array(uint8_t *data, uint8_t length){
	for(uint8_t i = 0; i < length; i++){
		send_byte(data[i]);
	}
}

// fifo_push_value
// push a value to the fifo.
// if the fifo is full, it will wait until it is ready to push.
void fifo_push_value(uint8_t data){
	while(!fifo_is_ready_to_push()){
		// wait until the fifo is ready to push.
	}
	push_value(data);
}

// fifo_push_array
// push an array to the fifo.
// if the fifo is full, it will wait until it is ready to push.
void fifo_push_array(uint8_t *data, uint8_t length){
	for(uint8_t i = 0; i < length; i++){
		fifo_push_value(data[i]);
	}
}

// fifo_push_string
// push a string to the fifo.
// if the fifo is full, it will wait until it is ready to push.
void fifo_push_string(char *string){
	uint8_t length = strlen(string);
	uint8_t *data = (uint8_t *)string;
	fifo_push_array(data, length);
}

// fifo_pop_value
// pop a value from the fifo.
// if the fifo is empty, it will wait until it is ready to pop.
uint8_t fifo_pop_value(void){
	while(!fifo_is_ready_to_pop()){
		// wait until the fifo is ready to pop.
	}
	return pop_value();
}

// fifo_pop_array
// pop an array from the fifo.
// if the fifo is empty, it will wait until it is ready to pop.
void fifo_pop_array(uint8_t *data, uint8_t length){
	for(uint8_t i = 0; i < length; i++){
		data[i] = fifo_pop_value();
	}
}

// fifo_pop_string
// pop a string from the fifo.
// if the fifo is empty, it will wait until it is ready to pop.
char *fifo_pop_string(void){
	uint8_t length = fifo_pop_value();
	uint8_t *data = (uint8_t *)malloc(length);
	fifo_pop_array(data, length);
	char *string = (char *)data;
	return string;
}

// fifo_is_empty
// check if the fifo is empty.
bool fifo_is_empty(void){
	return !fifo_is_ready_to_pop();
}

// fifo_is_full
// check if the fifo is full.
bool fifo_is_full(void){
	return !fifo_is_ready_to_push();
}

// fifo_is_ready_to_push
// check if the fifo is ready to push.
bool fifo_is_ready_to_push(void){
	return !(FIFO_STATUS & FIFO_STATUS_FULL);
}

// fifo_is_ready_to_pop
// check if the fifo is ready to pop.
bool fifo_is_ready_to_pop(void){
	return !(FIFO_STATUS & FIFO_STATUS_EMPTY);
}

// fifo_clear
// clear the fifo.
void fifo_clear(void){
	FIFO_STATUS = FIFO_STATUS_EMPTY;
}

// fifo_get_size
// get the size of the fifo.
uint8_t fifo_get_size(void){
	return FIFO_STATUS & FIFO_STATUS_SIZE;
}

// fifo_push_value
// push a value to the fifo.
// if the fifo is full, it will wait until it is ready to push.
void fifo_push_value(uint8_t data){
	while(!fifo_is_ready_to_push()){
		// wait until the fifo is ready to push.
	}
	push
// fifo_get_max_size
// get the max size of the fifo.
uint8_t fifo_get_max_size(void){
	return FIFO_STATUS & FIFO_STATUS_MAX_SIZE;
}

// fifo_set_max_size
// set the max size of the fifo.
void fifo_set_max_size(uint8_t size){
	FIFO_STATUS = (FIFO_STATUS & ~FIFO_STATUS_MAX_SIZE) | (size & FIFO_STATUS_MAX_SIZE);
}

// fifo_set_size
// set the size of the fifo.
void fifo_set_size(uint8_t size){
	FIFO_STATUS = (FIFO_STATUS & ~FIFO_STATUS_SIZE) | (size & FIFO_STATUS_SIZE);
}

uint8_t pop_value(){
	uint8_t data = FIFO_DATA;
	FIFO_DATA = 0;
	return data;
}

void push_value(uint8_t data){
	FIFO_DATA = data;
}

bool wait_until_ready_to_send(void){
	while(!(UART_STATUS & UART_STATUS_TX_READY)){
		// wait until the uart is ready to send.
	}
	return true;
}

void send_byte(uint8_t data){
	UART_DATA = data;
	wait_until_ready_to_send();
}

void send_array(uint8_t *data, uint8_t length){
	for(uint8_t i = 0; i < length; i++){
		send_byte(data[i]);
	}
}

