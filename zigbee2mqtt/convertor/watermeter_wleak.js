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
  metering: {
    key:['volume'],
    convertGet: async (entity, key, meta) => {
	    await entity.read('seMetering', ['currentSummDelivered']);
	  },
	  convertSet: async (entity, key, value, meta) => {
		  return null;
	  },
  },
};

const fzLocal = {
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


const definition = {
    zigbeeModel: ['Watermeter_TLSR8258'], // The model ID from: Device with modelID 'lumi.sens' is not supported.
    model: 'Watermeter TLSR8258', // Vendor model number, look on the device for a model number
    vendor: 'Slacky-DIY', // Vendor of the device (only used for documentation and startup logging)
    description: 'Water Meter', // Description of the device, copy from vendor site. (only used for documentation and startup logging)
    fromZigbee: [fz.battery, fz.ias_water_leak_alarm_1, fz.command_on, fz.command_off, fzLocal], // We will add this later
//    fromZigbee: [fz.battery, fzLocal], // We will add this later
    toZigbee: [tzLocal.metering, tz.currentsummdelivered, tzLocal.hot_water_config, tzLocal.cold_water_config, tzLocal.step_water_config],
    meta: {
        multiEndpoint: true
    },
    configure: async (device, coordinatorEndpoint, logger) => {
      const firstEndpoint = device.getEndpoint(1);
      await firstEndpoint.read('seMetering', ['currentSummDelivered']);
      await firstEndpoint.read('genPowerCfg', ['batteryVoltage', 'batteryPercentageRemaining']);
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
    icon: 'data:image/jpg;base64,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',
// Should be empty, unless device can be controlled (e.g. lights, switches).
    exposes: [e.battery(), e.battery_voltage(), e.water_leak(),
              exposes.numeric('volume', ea.STATE_GET).withUnit('Liters').withDescription('Hot water').withEndpoint('1'),
              exposes.numeric('volume', ea.STATE_GET).withUnit('Liters').withDescription('Cold water').withEndpoint('2'),
              exposes.numeric('hot_water_preset', ea.STATE_SET).withUnit('Liters').withDescription('Preset hot water'),
              exposes.numeric('cold_water_preset', ea.STATE_SET).withUnit('Liters').withDescription('Preset cold water'),
              exposes.numeric('step_water_preset', ea.STATE_SET).withUnit('Liters').withDescription('Preset step water')],
              
    ota: ota.zigbeeOTA,
};

module.exports = definition;
