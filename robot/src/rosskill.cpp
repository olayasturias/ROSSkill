#include <rosskill.h>
#include <ros.h>
#include <embedded_linux_hardware.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/Range.h>

class ImagePublisherWrapper {
    private:
        // Default OUTPUT_SIZE of NodeHandle is too small to send images, we have to create it manually.
        ros::NodeHandle_<EmbeddedLinuxHardware, 25, 25, 512, 32767> nh;
        sensor_msgs::CompressedImage image;
        ros::Publisher publisher;

    public:
        ImagePublisherWrapper(char *ip, char *topic): publisher(topic, &image) {
            nh.initNode(ip);
            nh.advertise(publisher);
            image.format = "jpeg";
        }
        void publishImage(unsigned char* data, int len) {
            image.data_length = len;
            image.data = (unsigned char*)data;
            publisher.publish(&image);
            nh.spinOnce();
        }
};


class RangePublisherWrapper {
    private:
        // Default OUTPUT_SIZE of NodeHandle is too small to send images, we have to create it manually.
        // Rosserial limits the number of Publishers and Subscribers to 25
        // NH params: Hardware type | Max publishers | Max subscribers | Input Buffer size | Output buffer size
        ros::NodeHandle_<EmbeddedLinuxHardware, 25, 25, 512, 32767> nh;
        sensor_msgs::Range range;
        ros::Publisher publisher;
    public:
        RangePublisherWrapper(char *ip, char *topic): publisher(topic, &range) {
            printf("initnode\n");
            nh.initNode(ip);
            nh.advertise(publisher);
            range.min_range = 0.1;
            range.max_range = 1.5;
        }
        void publishRange(float data) {
            range.range = data;
            publisher.publish(&range);
            nh.spinOnce();
            sleep(1);
        }
};

/* Wrapper functions */
// Image
extern "C" TopicPublisher* NewImagePublisher(char *ip, char *topic) {
    TopicPublisher* img_pub = new TopicPublisher();
    img_pub->wrapper = new ImagePublisherWrapper(ip, topic);
    return img_pub;
}

extern "C" void PublishImage(TopicPublisher* img_pub, unsigned char*data, int len) {
    ((ImagePublisherWrapper *)img_pub->wrapper)->publishImage(data, len);
}

extern "C" void DeleteImagePublisher(TopicPublisher* img_pub) {
    ImagePublisherWrapper* wrapper = (ImagePublisherWrapper *)(img_pub->wrapper);
    delete wrapper;
    delete img_pub;
}

// Range

extern "C" TopicPublisher* NewRangePublisher(char *ip, char *topic) {
    TopicPublisher* range_pub = new TopicPublisher();
    range_pub->wrapper = new RangePublisherWrapper(ip, topic);
    return range_pub;
}

extern "C" void PublishRange(TopicPublisher* range_pub, float data) {
    ((RangePublisherWrapper *)range_pub->wrapper)->publishRange(data);
}

extern "C" void DeleteRangePublisher(TopicPublisher* range_pub) {
    RangePublisherWrapper* wrapper = (RangePublisherWrapper *)(range_pub->wrapper);
    delete wrapper;
    delete range_pub;
}
