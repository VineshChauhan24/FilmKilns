/*
* Copyright (c) 2018-present, aliminabc@gmail.com.
*
* This source code is licensed under the MIT license found in the
* LICENSE file in the root directory of this source tree.
*/

#include "AlAlignCropOperateModel.h"
#include "AlMath.h"

AlAlignCropOperateModel::AlAlignCropOperateModel() : AlAbsOperateModel() {

}

AlAlignCropOperateModel::AlAlignCropOperateModel(const AlAlignCropOperateModel &o)
        : AlAbsOperateModel(), rotation(o.rotation) {

}

AlAlignCropOperateModel::~AlAlignCropOperateModel() {

}

HwResult AlAlignCropOperateModel::measure(AlImgLayerDescription &layer,
                                          AlImageLayerDrawModel *description) {
    ///scale = (h / w / tan(PI / 2 - alpha) + 1) * cos(alpha)
    AlSize size = layer.getSize();
    double alpha = rotation.toDouble();
    double scale =
            (size.height / (double) size.width / abs(tan(AlMath::PI / 2.0f - alpha * AlMath::PI))
             + 1.0f) * abs(cos(alpha * AlMath::PI));
    layer.setScale(static_cast<float>(scale), static_cast<float>(scale));
    layer.setRotation(rotation);
    layer.setPosition(0.0f, 0.0f);
    return Hw::SUCCESS;
}

void AlAlignCropOperateModel::setRotation(AlRational &r) {
    rotation.num = r.num;
    rotation.den = r.den;
}