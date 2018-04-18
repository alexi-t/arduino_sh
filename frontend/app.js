const express = require('express')
const app = express()

const iotHub = require('azure-iothub');

const hubClient = iotHub.Client.fromConnectionString('HostName=sh-bathroom-0001.azure-devices.net;SharedAccessKeyName=service;SharedAccessKey=tRi/UcvbcptgKPzfllU7tMao4Iq8ayekCRseAvkx2pM=');

const deviceId = 'wemosd1';

hubClient.open(function(result) {
    console.log(JSON.stringify(result));
});

app.get('/', (req, res) => res.send('Hello World!'))

app.post('/enableLight', function (req, res) {

    hubClient.invokeDeviceMethod(deviceId, { methodName: 'enableLight' });

    res.statusCode = 200;
    res.send('Got a POST request');
});

app.post('/disableLight', function (req, res) {
    
    hubClient.invokeDeviceMethod(deviceId, { methodName: 'disableLight' });

    res.statusCode = 200;
    res.send('Got a POST request');
});

app.listen(3000, () => console.log('Example app listening on port 3000!'))