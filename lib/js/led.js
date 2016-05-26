class lightLED{
	constructor(statusLED) {
            this.statusLED = statusLED;
    }

    get status(){
    	return this.statusLED;
    }
}