#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void * wrapper; // TopicPublisherWrapper, because we can't use any c++ class directly in cgo.
}TopicPublisher;

TopicPublisher* NewImagePublisher(char *ip, char *topic);
void PublishImage(TopicPublisher* pub, unsigned char*data, int len);
void DeleteImagePublisher(TopicPublisher* pub);

TopicPublisher* NewRangePublisher(char *ip, char *topic);
void PublishRange(TopicPublisher* pub, float data);
void DeleteRangePublisher(TopicPublisher* pub);

#ifdef __cplusplus
}
#endif
