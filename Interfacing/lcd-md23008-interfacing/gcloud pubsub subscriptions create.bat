gcloud pubsub subscriptions create     
projects/august-cascade-320302/subscriptions/my-subscription     
--topic=projects/august-cascade-320302/topics/iot-data


 node cloudiot_mqtt_example_nodejs.js \
    mqttDeviceDemo \
    --projectId=august-cascade-320302 \
    --cloudRegion=us-central1 \
    --registryId=rasp-registry \
    --deviceId=raspi-device \
    --privateKeyFile=rsa_private.pem \
    --serverCertFile=roots.pem \
    --numMessages=25 \
    --algorithm=RS256

gcloud pubsub subscriptions pull --auto-ack projects/august-cascade-320302/subscriptions/my-subscription

gcloud pubsub subscriptions pull --auto-ack projects/august-cascade-320302/subscriptions/my-subscription

