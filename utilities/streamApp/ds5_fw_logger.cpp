/*
 * Copyright © 2018 Intel Corporation. All rights reserved.
 *
 * The source code contained or described herein and all documents related to the source code
 * ("Material") are owned by Intel Corporation or its suppliers or licensors. Title to the
 * Material remains with Intel Corporation or its suppliers and licensors. The Material may
 * contain trade secrets and proprietary and confidential information of Intel Corporation
 * and its suppliers and licensors, and is protected by worldwide copyright and trade secret
 * laws and treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or disclosed in any way
 * without Intel’s prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual property right
 * is granted to or conferred upon you by disclosure or delivery of the Materials, either
 * expressly, by implication, inducement, estoppel or otherwise. Any license under such
 * intellectual property rights must be express and approved by Intel in writing.
 */

#include <inttypes.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <linux/videodev2.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>

using namespace std;

#define DS5_CAMERA_CID_BASE	(V4L2_CTRL_CLASS_CAMERA | 0x4000)

#define DS5_CAMERA_CID_LOG		(DS5_CAMERA_CID_BASE+0)

struct ds5_fw_log_msg
{
	uint8_t magic;
	uint8_t severity:  5;
	uint8_t thread_id: 3;
	uint16_t file_id:  11;
	uint16_t group_id:  5;
	uint16_t event_id;
	uint16_t line_id:  12;
	uint16_t seq_id:    4;
	uint16_t param0;
	uint16_t param1;
	uint32_t param2;
	uint32_t timestamp;
} __attribute__((packed));

#define HEADER_SIZE 4

int main(int argc, char *argv[])
{
	uint8_t log[1024];
	ssize_t size = sizeof(log) - HEADER_SIZE;
	struct ds5_fw_log_msg *msg = (struct ds5_fw_log_msg *)(log + HEADER_SIZE);
	struct v4l2_ext_control ctrl {0};
	ctrl.id = DS5_CAMERA_CID_LOG;
	ctrl.size = sizeof(log);
	ctrl.p_u8 = log;

	struct v4l2_ext_controls ext {0};
	ext.ctrl_class = V4L2_CTRL_CLASS_CAMERA;
	ext.controls = &ctrl;
	ext.count = 1;

	unsigned int i;
	int ret, fd = open(argv[1], O_RDWR);
	if (fd < 0) {
		printf("open failed, errno %d\n", errno);
		return errno;
	}

	while (1) {
		this_thread::sleep_for(100ms);

		ret = ioctl(fd, VIDIOC_G_EXT_CTRLS, &ext);
		if (ret < 0) {
			printf("ioctl failed, errno %d\n", errno);
		}

		size = sizeof(log) - HEADER_SIZE;
		msg = (struct ds5_fw_log_msg *)(log + HEADER_SIZE);

		for (int j = 0; size > sizeof(*msg); j++, msg++, size -= sizeof(*msg)) {
			if (msg->magic != 0xa0)
				continue;
			for (i = 4; i < sizeof(ds5_fw_log_msg) + HEADER_SIZE; i++) {
				cout << uppercase << std::setfill ('0') << setw(2) << std::hex << (int)log[j*sizeof(ds5_fw_log_msg)+i] << " ";
			}
		cout << endl;
		}

		/*for (i = 0; size > sizeof(*msg); i++, msg++, size -= sizeof(*msg)) {
			if (msg->magic != 0xa0)
				continue;

			printf("[FW LOG] %u %u %u %u %u %u %u %u %u %u %u\n",
			msg->severity, msg->thread_id, msg->file_id,
			msg->group_id, msg->event_id, msg->line_id, msg->seq_id,
			msg->param0, msg->param1, msg->param2, msg->timestamp);
		}*/
	}

	close(fd);
	return 0;
}
