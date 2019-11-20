/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#ifndef HWVC_ANDROID_ALIMAGELAYERMANAGER_H
#define HWVC_ANDROID_ALIMAGELAYERMANAGER_H

#include "Object.h"
#include "AlImageLayer.h"
#include "AlImageLayerModel.h"
#include "TextureAllocator.h"
#include <list>
#include <map>

al_class(AlImageLayerManager) {
public:
    AlImageLayerManager();

    ~AlImageLayerManager();

    void update(std::list<AlImageLayerModel *> *list, TextureAllocator *texAllocator);

    int32_t size();

    AlImageLayer *getLayout(int32_t index);

private:
    AlImageLayerManager(const AlImageLayerManager &e) : Object() {};

    bool _newLayer(AlImageLayerModel *model, TextureAllocator *texAllocator);

    bool _found(AlImageLayerModel *model);

private:
    std::map<AlImageLayerModel *, AlImageLayer *> mLayers;
};


#endif //HWVC_ANDROID_ALIMAGELAYERMANAGER_H