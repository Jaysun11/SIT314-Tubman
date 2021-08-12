var dataOut;

setInterval(sendData, 5000); //time is in ms

function sendData() {

    const http = require('http');

    var post_options = {
        host: '13.55.85.160',
        //host: '13.55.54.107',
        port: '3000',
        method: 'POST',
    };

    generateData();

    const req = http.request(post_options, res => {
        console.log(`statusCode: ${res.statusCode}`)

        res.on('data', d => {
            process.stdout.write(d)
        })
    })

    req.on('error', error => {
        console.error(error)
    })

    req.write(dataOut)
    req.end()
    console.log("Sent data to server: " + dataOut);
}

function generateData() {

    const sensordata = {
        id: 0,
        name: "temperaturesensor",
        address: "25 Stainer St, Willagee",
        time: Date.now(),
        temperature: 20
    }

    const low = 10;
    const high = 40;
    reading = Math.floor(Math.random() * (high - low) + low);
    sensordata.temperature = reading;

    dataOut = JSON.stringify(sensordata);

}





