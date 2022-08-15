#pragma once
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

extern WiFiClientSecure net;
extern PubSubClient client;

// Constants
extern const char *AWS_IOT_PUBLISH_IMAGES_TOPIC;
extern const char *AWS_IOT_PUBLISH_PARAMS_TOPIC;
extern const char *AWS_IOT_SUBSCRIBE_TOPIC;

// handle incoming messages
void messageHandler(char *topic, byte *payload, unsigned int length);
// connect to AWS
void connectAWS();
// Send image to AWS s3 bucket (work to do)
void send_image(uint8_t *im, size_t size);
// send parameters to S3 buckets (resolution, blocksize etc.)
void send_params();
