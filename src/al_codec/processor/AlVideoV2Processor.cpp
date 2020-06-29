/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "AlVideoV2Processor.h"

#include <utility>
#include "AlUTimeline.h"
#include "AlUSequence.h"
#include "AlUAudios.h"
#include "HwSpeaker.h"

#define TAG "AlVideoV2Processor"

AlVideoV2Processor::AlVideoV2Processor() : AlAbsProcessor(TAG) {
    registerAnUnit(new AlUTimeline(ALIAS_TIMELINE));
    registerAnUnit(new AlUSequence(ALIAS_SEQUENCE));
    registerAnUnit(new AlUAudios(ALIAS_AUDIOS));
    registerAnUnit(new HwSpeaker(ALIAS_SPEAKER));

    al_reg_msg(MSG_TIMELINE_PROGRESS_NOTIFY, AlVideoV2Processor::_onTimelineInUS);
    al_reg_msg(MSG_SEQUENCE_TRACK_ADD_DONE, AlVideoV2Processor::_onAddTrackDone);
}

AlVideoV2Processor::~AlVideoV2Processor() {

}

void AlVideoV2Processor::onCreate() {
    AlAbsProcessor::onCreate();
}

void AlVideoV2Processor::onDestroy() {
    AlAbsProcessor::onDestroy();
}

bool AlVideoV2Processor::_onTimelineInUS(AlMessage *msg) {
    auto progress = msg->arg1;
    auto duration = msg->arg2;
    playProgressListener(progress * duration / 1e6, duration);
    return true;
}

bool AlVideoV2Processor::_onAddTrackDone(AlMessage *msg) {
    mCurTrackID = msg->arg1;
    mCurTrackIDLock.notify();
    return true;
}

int32_t AlVideoV2Processor::addTrack(AlCodec::kMediaType type, std::string path,
                                     int64_t seqInInUS, int64_t seqOutInUS,
                                     int64_t trimInInUS, int64_t trimOutInUS) {
    AlFileDescriptor descriptor(std::move(path));
    auto clip = std::make_shared<AlMediaClip>(0, descriptor);
    clip->setSeqIn(seqInInUS);
    clip->setTrimIn(trimInInUS);

    AlMessage *msg = AlMessage::obtain(MSG_SEQUENCE_TRACK_ADD);
    msg->arg1 = (int32_t) type;
    msg->sp = clip;
    postMessage(msg);
    mCurTrackIDLock.wait();
    return mCurTrackID;
}

void AlVideoV2Processor::start() {
    postMessage(AlMessage::obtain(MSG_TIMELINE_START));
}

void AlVideoV2Processor::pause() {
    postMessage(AlMessage::obtain(MSG_TIMELINE_PAUSE));
}

void AlVideoV2Processor::seek(int64_t timeInUS) {
    auto *msg = AlMessage::obtain(MSG_TIMELINE_SEEK);
    msg->arg2 = timeInUS;
    postMessage(msg);
}

void AlVideoV2Processor::setPlayProgressListener(function<void(int64_t, int64_t)> listener) {
    playProgressListener = std::move(listener);
}
