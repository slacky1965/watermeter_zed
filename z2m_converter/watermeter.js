const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const ota = require('zigbee-herdsman-converters/lib/ota');
const extend = require('zigbee-herdsman-converters/lib/extend');
const utils = require('zigbee-herdsman-converters/lib/utils');
const globalStore = require('zigbee-herdsman-converters/lib/store');
const { postfixWithEndpointName, precisionRound } = require('zigbee-herdsman-converters/lib/utils.js') 
const e = exposes.presets;
const ea = exposes.access;

const tzLocal = {
  hot_water_config: {
    key: ['hot_water_preset'],
      convertSet: async (entity, key, rawValue, meta) => {
			  const endpoint = meta.device.getEndpoint(3);
        const lookup = {'OFF': 0x00, 'ON': 0x01};
        const value = lookup.hasOwnProperty(rawValue) ? lookup[rawValue] : parseInt(rawValue, 10);
        const payloads = {
          hot_water_preset: ['seMetering', {0xf000: {value, type: 0x23}}],
        };
        await endpoint.write(payloads[key][0], payloads[key][1]);
        return {
          state: {[key]: rawValue},
        };
      },
    
  },
  cold_water_config: {
    key: ['cold_water_preset'],
      convertSet: async (entity, key, rawValue, meta) => {
			  const endpoint = meta.device.getEndpoint(3);
        const lookup = {'OFF': 0x00, 'ON': 0x01};
        const value = lookup.hasOwnProperty(rawValue) ? lookup[rawValue] : parseInt(rawValue, 10);
        const payloads = {
				  cold_water_preset: ['seMetering', {0xf001: {value, type: 0x23}}],
        };
        await endpoint.write(payloads[key][0], payloads[key][1]);
        return {
          state: {[key]: rawValue},
        };
      },
    
  },
  step_water_config: {
    key: ['step_water_preset'],
      convertSet: async (entity, key, rawValue, meta) => {
			  const endpoint = meta.device.getEndpoint(3);
        const lookup = {'OFF': 0x00, 'ON': 0x01};
        const value = lookup.hasOwnProperty(rawValue) ? lookup[rawValue] : parseInt(rawValue, 10);
        const payloads = {
          step_water_preset: ['seMetering', {0xf002: {value, type: 0x21}}],
        };
        await endpoint.write(payloads[key][0], payloads[key][1]);
        return {
          state: {[key]: rawValue},
        };
      },
    
  },
};

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
    toZigbee: [toZigbee_Metering, tz.currentsummdelivered, tzLocal.hot_water_config, tzLocal.cold_water_config, tzLocal.step_water_config],
    meta: {
        multiEndpoint: true
    },
    configure: async (device, coordinatorEndpoint, logger) => {
      const firstEndpoint = device.getEndpoint(1);
      await firstEndpoint.read('seMetering', ['currentSummDelivered']);
      await reporting.bind(firstEndpoint, coordinatorEndpoint, ['genPowerCfg', 'seMetering' ]);
      const overides = {min: 300, max: 3600, change: 0};
      await reporting.batteryVoltage(firstEndpoint, overides);
      await reporting.batteryPercentageRemaining(firstEndpoint, overides);
      await reporting.currentSummDelivered(firstEndpoint, {min: 0, max: 300, change: 0});
      const secondEndpoint = device.getEndpoint(2);
      await secondEndpoint.read('seMetering', ['currentSummDelivered']);
      await reporting.bind(secondEndpoint, coordinatorEndpoint, ['seMetering' ]);
      await reporting.currentSummDelivered(secondEndpoint, {min: 0, max: 300, change: 0});
    },
// Should be empty, unless device can be controlled (e.g. lights, switches).
    exposes: [e.battery(), e.battery_voltage(), 
              exposes.numeric('volume', ea.STATE_GET).withUnit('Liters').withDescription('Hot water').withEndpoint('1'),
              exposes.numeric('volume', ea.STATE_GET).withUnit('Liters').withDescription('Cold water').withEndpoint('2'),
              exposes.numeric('hot_water_preset', ea.STATE_SET).withUnit('Liters').withDescription('Preset hot water'),
              exposes.numeric('cold_water_preset', ea.STATE_SET).withUnit('Liters').withDescription('Preset cold water'),
              exposes.numeric('step_water_preset', ea.STATE_SET).withUnit('Liters').withDescription('Preset step water')],
              
    ota: ota.zigbeeOTA,
};

module.exports = definition;