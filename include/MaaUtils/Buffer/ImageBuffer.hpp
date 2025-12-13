#pragma once

#include "BufferTypes.hpp"
#include "ListBuffer.hpp"
#include "MaaUtils/Conf.h"

MAA_SUPPRESS_CV_WARNINGS_BEGIN
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
MAA_SUPPRESS_CV_WARNINGS_END

MAA_NS_BEGIN

class ImageBuffer : public MaaImageBuffer
{
public:
    ImageBuffer() = default;

    ImageBuffer(cv::Mat image)
        : image_(std::move(image))
    {
    }

    ImageBuffer(ImageEncodedBuffer buffer) { from_encoded(std::move(buffer)); }

    virtual ~ImageBuffer() override = default;

    virtual bool empty() const override { return image_.empty(); }

    virtual void clear() override
    {
        image_.release();
        encoded_cache_.clear();
    }

    virtual void* raw_data() const override { return image_.data; }

    virtual int32_t width() const override { return image_.cols; }

    virtual int32_t height() const override { return image_.rows; }

    virtual int32_t channels() const override { return image_.channels(); }

    virtual int32_t type() const override { return image_.type(); }

    virtual uint8_t* encoded() const override
    {
        encode();
        return encoded_cache_.data();
    }

    virtual size_t encoded_size() const override
    {
        encode();
        return encoded_cache_.size();
    }

    virtual const cv::Mat& get() const override { return image_; }

    virtual void set(cv::Mat image) override
    {
        dirty_ = true;
        image_ = image.clone();
    }

    virtual void set(ImageEncodedBuffer buffer) override { from_encoded(std::move(buffer)); }

private:
    void from_encoded(ImageEncodedBuffer buffer)
    {
        image_ = cv::imdecode(buffer, cv::IMREAD_COLOR);
        encoded_cache_ = std::move(buffer);
        dirty_ = false;
    }

    void encode() const
    {
        if (!dirty_) {
            return;
        }

        if (image_.empty()) {
            encoded_cache_.clear();
        }
        else {
            cv::imencode(".png", image_, encoded_cache_);
        }
        dirty_ = false;
    }

    cv::Mat image_;
    mutable bool dirty_ = true;
    mutable ImageEncodedBuffer encoded_cache_;
};

MAA_NS_END

struct MaaImageListBuffer : public MAA_NS::ListBuffer<MAA_NS::ImageBuffer>
{
    virtual ~MaaImageListBuffer() override = default;
};
