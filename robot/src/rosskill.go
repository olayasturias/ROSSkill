// ROS Skill implements a skill that shows how to publish images to an ROS topic via rosserial.

package example

// #include "rosskill.h"
import "C"

import (
	"mind/core/framework/drivers/media"
	"mind/core/framework/drivers/distance"

	"mind/core/framework/log"
	"mind/core/framework/skill"

	"bytes"
	"image/jpeg"
)

const (
	rosMasterIP = "192.168.251.106" // ROS_MASTER_IP need to be modified manually
	rosTopic_img    = "/image/compressed"
	rosTopic_dist    = "/distance"
)

var jpegOption = &jpeg.Options{10} // Size of rosserial messages can not exceed 32767, so we have to use low quality images here.

type rosskill struct {
	skill.Base
	imagePublisher    *C.TopicPublisher
	distancePublisher *C.TopicPublisher
	stop           chan bool
}

func NewSkill() skill.Interface {
	return &rosskill{
		imagePublisher: C.NewImagePublisher(C.CString(rosMasterIP), C.CString(rosTopic_img)),
		distancePublisher: C.NewRangePublisher(C.CString(rosMasterIP), C.CString(rosTopic_dist)),
		stop:           make(chan bool),
	}
}

func (d *rosskill) publishImages() {
	for {
		select {
		case <-d.stop:
			break
		default:
		}
		var b bytes.Buffer
		img := media.SnapshotRGBA()
		jpeg.Encode(&b, img, jpegOption)
		C.PublishImage(
			d.imagePublisher,
			(*C.uchar)(C.CBytes(b.Bytes())),
			C.int(len(b.Bytes())),
		)
		log.Info.Println("Sent Image with length:", len(b.Bytes()))
	}
}

func (d *rosskill) publishDistance() {
	for {
		select {
		case <-d.stop:
			break
		default:
		}
		dist, err := distance.Value()
		dist = dist/1000 // convert mm to m
		dist32 := float32(dist)
		if err != nil {
		log.Error.Println(err)
		}
		C.PublishRange(d.distancePublisher,C.float(dist32))
		log.Info.Println("Sent Distance topic", C.float(dist32))
	}
}

func (d *rosskill) OnStart() {
	// err := media.Start()
  err2 := distance.Start()
	// if err != nil {
	// 	log.Error.Println("Media start err:", err)
	// 	return
	// }
	if err2 != nil {
		log.Error.Println("Distance start err:", err2)
		return
	}
	//d.publishImages()
	d.publishDistance()
}

func (d *rosskill) OnClose() {
	d.stop <- true
	//C.DeleteImagePublisher(d.imagePublisher)
	C.DeleteRangePublisher(d.distancePublisher)
	// media.Close()
	distance.Close()
}
