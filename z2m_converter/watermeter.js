const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const extend = require('zigbee-herdsman-converters/lib/extend');
const utils = require('zigbee-herdsman-converters/lib/utils');
const globalStore = require('zigbee-herdsman-converters/lib/store');
const { postfixWithEndpointName, precisionRound } = require('zigbee-herdsman-converters/lib/utils.js') 
const e = exposes.presets;
const ea = exposes.access;

const fromZigbee_Metering = {
    cluster: 'seMetering',
    type: ['attributeReport', 'readResponse'],
    convert: (model, msg, publish, options, meta) => {
        const payload = {};
        if (msg.data.hasOwnProperty('currentSummDelivered')) {
            const data = msg.data['currentSummDelivered'];
            const value = (parseInt(data[0]) << 32) + parseInt(data[1]);
            let volume = value; // * 0.001; //* factor;
				    payload[postfixWithEndpointName('volume', msg, model, meta)] = precisionRound(volume, 2);
        } else {
				    payload[postfixWithEndpointName('volume', msg, model, meta)] = 0;
			  }

        return payload;
    }
}
	
const toZigbee_Metering = {
    key:['volume'],
    convertGet: async (entity, key, meta) => {
			await entity.read('seMetering', ['currentSummDelivered']);
		},
		convertSet: async (entity, key, value, meta) => {
			return null;
		}
}

const definition = {
    zigbeeModel: ['Watermeter_TLSR8258'], // The model ID from: Device with modelID 'lumi.sens' is not supported.
    model: 'Watermeter TLSR8258', // Vendor model number, look on the device for a model number
    vendor: 'DIY', // Vendor of the device (only used for documentation and startup logging)
    description: 'Water Meter', // Description of the device, copy from vendor site. (only used for documentation and startup logging)
    fromZigbee: [fz.battery, fromZigbee_Metering], // We will add this later
    toZigbee: [toZigbee_Metering, tz.currentsummdelivered],
    meta: {
        multiEndpoint: true
    },
    configure: async (device, coordinatorEndpoint, logger) => {
      const firstEndpoint = device.getEndpoint(1);
      await reporting.bind(firstEndpoint, coordinatorEndpoint, ['genPowerCfg', 'seMetering' ]);
      const overides = {min: 300, max: 3600, change: 0};
      await reporting.batteryVoltage(firstEndpoint, overides);
      await reporting.batteryPercentageRemaining(firstEndpoint, overides);
      await reporting.currentSummDelivered(firstEndpoint, {min: 0, max: 3600, change: 0});
      const secondEndpoint = device.getEndpoint(2);
      await reporting.bind(secondEndpoint, coordinatorEndpoint, ['seMetering' ]);
      await reporting.currentSummDelivered(secondEndpoint, {min: 0, max: 3600, change: 0});
    },
// Should be empty, unless device can be controlled (e.g. lights, switches).
    exposes: [e.battery(), e.battery_voltage()], // Defines what this device exposes, used for e.g. Home Assistant discovery and in the frontend
};

module.exports = definition;