class switch{
	constructor(status) {
            this.id				= id;
            this.status 		= status;
            this.statusOnOff	= statusOnOff;
    }

    get status(){
    	return this.status;
    }

    set OnOff(let onOff){
    	return this.statusOnOff = onOff;
    }
}