const mongoose = require('mongoose');

const port = 3000;

var sensordata = {
    id: 0,
    name: "temperaturesensor",
    address: "25 Stainer St, Willagee",
    time: Date.now(),
    temperature: 20
}

const http = require('http')


const server = http.createServer((req, res) => {
    res.statusCode = 200
    req.on('data', function (chunk) {
        console.log('Recieved Data: ' + chunk);

        //Open mongoose connection
        console.log("Open connection to MongoDB");

        const Sensor = require('./models/sensor');

        mongoose.connect('mongodb+srv://Jason:SIT314@cluster0.wdbyi.mongodb.net/SIT31453D?retryWrites=true&w=majority', { useNewUrlParser: true, useUnifiedTopology: true });

        sensordata = JSON.parse(chunk);
        const newSensor = new Sensor({
            id: sensordata.id,
            name: sensordata.name,
            address: sensordata.address,
            time: sensordata.time,
            temperature: sensordata.temperature
        });

        newSensor.save().then(doc => {
            console.log("Uploaded data to MongoDB");
            console.log(doc);
        }).then(() => {
            mongoose.connection.close();

        });
    });

}).listen(port, () => {
    console.log(`Server running at port ${port}`)
})
