/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2025 Winlin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <srs_utest_workflow_rtc_publishstream.hpp>

#include <srs_app_rtc_conn.hpp>
#include <srs_app_rtc_source.hpp>
#include <srs_kernel_error.hpp>
#include <srs_utest_ai11.hpp>
#include <srs_utest_manual_mock.hpp>

// This test is used to verify the basic workflow of the RTC publish stream.
// It's finished with the help of AI, but each step is manually designed
// and verified. So this is not dominated by AI, but by humanbeing.
VOID TEST(BasicWorkflowRtcPublishStreamTest, ManuallyVerify)
{
    srs_error_t err;

    // Create mock objects for dependencies
    MockAppConfig mock_config;
    MockRtcSourceManager mock_rtc_sources;
    MockAppStatistic mock_stat;
    MockRtcAsyncCallRequest mock_request("test.vhost", "live", "stream1");
    MockRtcAsyncTaskExecutor mock_exec;
    MockExpire mock_expire;
    MockRtcPacketReceiver mock_receiver;
    MockRtcTrackDescriptionFactory track_factory;
    MockRtcFormat mock_format;
    SrsContextId cid;
    cid.set_value("test-publish-stream-cid");

    // Create RTC publish stream - use real pli_worker_
    SrsUniquePtr<SrsRtcPublishStream> publish_stream(new SrsRtcPublishStream(&mock_exec, &mock_expire, &mock_receiver, cid));

    // Mock the publish stream object
    if (true) {
        // Inject mock dependencies
        publish_stream->config_ = &mock_config;
        publish_stream->rtc_sources_ = &mock_rtc_sources;
        publish_stream->stat_ = &mock_stat;

        // Replace the real format_ with mock format
        srs_freep(publish_stream->format_);
        publish_stream->format_ = &mock_format;
    }

    // Create stream description with audio and video tracks
    if (true) {
        SrsUniquePtr<SrsRtcSourceDescription> stream_desc(track_factory.create_stream_description());

        // Initialize the publish stream (it will take ownership of track descriptions)
        HELPER_EXPECT_SUCCESS(publish_stream->initialize(&mock_request, stream_desc.get()));

        // Check the tracks, should be one audio track
        EXPECT_EQ(publish_stream->audio_tracks_.size(), 1);
        // Check the tracks, should be one video track
        EXPECT_EQ(publish_stream->video_tracks_.size(), 1);

        // Test: First call to start() should succeed
        HELPER_EXPECT_SUCCESS(publish_stream->start());

        // Verify is_sender_started_ flag is set
        EXPECT_TRUE(publish_stream->is_sender_started_);

        // Wait for coroutine to start. Normally it should be ready and stopped at wait
        // for PLI requests.
        srs_usleep(1 * SRS_UTIME_MILLISECONDS);
    }

    // Request a PLI about the video ssrc to the publisher.
    if (true) {
        uint32_t video_ssrc = track_factory.video_ssrc_;
        publish_stream->request_keyframe(video_ssrc, cid);

        // Wait for coroutine to process the request
        srs_usleep(1 * SRS_UTIME_MILLISECONDS);

        // Verify the PLI is sent out
        EXPECT_EQ(mock_receiver.send_rtcp_fb_pli_count_, 1);
    }

    // Stop the publish stream
    publish_stream->stop();

    // Before destroying publish_stream, set format_ to NULL to prevent double-free
    // since mock_format is a stack variable
    publish_stream->format_ = NULL;
}

// [OpenSight fork] Decode the target bitrate from a REMB packet.
static uint64_t srs_test_remb_bitrate(const std::string &pkt)
{
    const unsigned char *p = (const unsigned char *)pkt.data();
    uint8_t exp = (uint8_t)(p[17] >> 2);
    uint32_t mantissa = ((uint32_t)(p[17] & 0x03) << 16) | ((uint32_t)p[18] << 8) | (uint32_t)p[19];
    return ((uint64_t)mantissa) << exp;
}

// [OpenSight fork] The dependencies of a publish stream with the given REMB configuration.
class RembTestEnv
{
public:
    MockAppConfig config_;
    MockRtcSourceManager rtc_sources_;
    MockAppStatistic stat_;
    MockRtcAsyncCallRequest req_;
    MockRtcAsyncTaskExecutor exec_;
    MockExpire expire_;
    MockRtcPacketReceiver receiver_;
    MockRtcTrackDescriptionFactory factory_;
    MockRtcFormat format_;
    SrsRtcPublishStream *stream_;
    SrsContextId cid_;

public:
    RembTestEnv() : req_("test.vhost", "live", "stream1")
    {
        cid_.set_value("remb-test-cid");
        stream_ = new SrsRtcPublishStream(&exec_, &expire_, &receiver_, cid_);
        stream_->config_ = &config_;
        stream_->rtc_sources_ = &rtc_sources_;
        stream_->stat_ = &stat_;
        srs_freep(stream_->format_);
        stream_->format_ = &format_;
    }
    virtual ~RembTestEnv()
    {
        stream_->format_ = NULL;
        srs_freep(stream_);
    }
};

VOID TEST(BasicWorkflowRtcPublishStreamTest, SendRembTargetForNegotiatedVideo)
{
    srs_error_t err;

    RembTestEnv env;
    env.config_.set_rtc_bitrate(1000000);

    SrsUniquePtr<SrsRtcSourceDescription> stream_desc(env.factory_.create_stream_description());
    // The SDP negotiates goog-remb on the first video payload.
    stream_desc->video_track_descs_[0]->media_->rtcp_fbs_.push_back("goog-remb");

    HELPER_EXPECT_SUCCESS(env.stream_->initialize(&env.req_, stream_desc.get()));
    HELPER_EXPECT_SUCCESS(env.stream_->start());

    // The startup feedbacks ramp as bitrate/4, /3, /2 and /1, then keep the full target. Note that the
    // 18-bit mantissa quantizes the /3 feedback to 333332 bps.
    uint64_t expected[] = {250000, 333332, 500000, 1000000, 1000000};

    // Four startup feedbacks on the RTP path, then one periodic refresh on the timer.
    for (int i = 0; i < 4; i++) {
        HELPER_EXPECT_SUCCESS(env.stream_->send_rtcp_remb());

        ASSERT_EQ((size_t)(i + 1), env.receiver_.sent_rtcp_packets_.size());
        std::string pkt = env.receiver_.sent_rtcp_packets_.back();

        // Verify the actual PSFB REMB packet on the wire.
        ASSERT_EQ(24u, pkt.size());
        const unsigned char *p = (const unsigned char *)pkt.data();
        EXPECT_EQ(0x8F, p[0]);
        EXPECT_EQ(206, p[1]);
        EXPECT_EQ(0, p[2]);
        EXPECT_EQ(5, p[3]);
        EXPECT_EQ('R', p[12]);
        EXPECT_EQ('E', p[13]);
        EXPECT_EQ('M', p[14]);
        EXPECT_EQ('B', p[15]);
        EXPECT_EQ(1, p[16]);
        EXPECT_EQ(env.factory_.video_ssrc_, (uint32_t)((p[20] << 24) | (p[21] << 16) | (p[22] << 8) | p[23]));
        EXPECT_EQ(expected[i], srs_test_remb_bitrate(pkt));
    }

    HELPER_EXPECT_SUCCESS(env.stream_->timer_rtcp_->on_timer(1 * SRS_UTIME_SECONDS));

    ASSERT_EQ(5u, env.receiver_.sent_rtcp_packets_.size());
    EXPECT_EQ(1000000, srs_test_remb_bitrate(env.receiver_.sent_rtcp_packets_.back()));
}

VOID TEST(BasicWorkflowRtcPublishStreamTest, SkipRembWhenNotNegotiatedOrNoTarget)
{
    srs_error_t err;

    // A target without goog-remb negotiation never sends feedback.
    if (true) {
        RembTestEnv env;
        env.config_.set_rtc_bitrate(1000000);

        SrsUniquePtr<SrsRtcSourceDescription> stream_desc(env.factory_.create_stream_description());
        HELPER_EXPECT_SUCCESS(env.stream_->initialize(&env.req_, stream_desc.get()));

        HELPER_EXPECT_SUCCESS(env.stream_->send_rtcp_remb());
        EXPECT_EQ(0, env.receiver_.send_rtcp_count_);
    }

    // Negotiated goog-remb without any target never sends feedback.
    if (true) {
        RembTestEnv env;
        env.config_.set_rtc_bitrate(0);

        SrsUniquePtr<SrsRtcSourceDescription> stream_desc(env.factory_.create_stream_description());
        stream_desc->video_track_descs_[0]->media_->rtcp_fbs_.push_back("goog-remb");
        HELPER_EXPECT_SUCCESS(env.stream_->initialize(&env.req_, stream_desc.get()));

        HELPER_EXPECT_SUCCESS(env.stream_->send_rtcp_remb());
        EXPECT_EQ(0, env.receiver_.send_rtcp_count_);
    }

    // An audio-only publisher has no video SSRC to send feedback for.
    if (true) {
        RembTestEnv env;
        env.config_.set_rtc_bitrate(1000000);

        SrsUniquePtr<SrsRtcSourceDescription> stream_desc(new SrsRtcSourceDescription());
        stream_desc->audio_track_desc_ = env.factory_.create_audio_track(env.factory_.audio_ssrc_, "audio-track-1", "0");
        HELPER_EXPECT_SUCCESS(env.stream_->initialize(&env.req_, stream_desc.get()));

        EXPECT_TRUE(env.stream_->video_tracks_.empty());
        HELPER_EXPECT_SUCCESS(env.stream_->send_rtcp_remb());
        EXPECT_EQ(0, env.receiver_.send_rtcp_count_);
    }
}

VOID TEST(BasicWorkflowRtcPublishStreamTest, RequestBitrateOverridesVhostForRemb)
{
    srs_error_t err;

    // A valid per-request target keeps priority over the per-vhost default.
    RembTestEnv env;
    env.config_.set_rtc_bitrate(1000000);
    env.req_.bitrate_ = 128000;

    SrsUniquePtr<SrsRtcSourceDescription> stream_desc(env.factory_.create_stream_description());
    stream_desc->video_track_descs_[0]->media_->rtcp_fbs_.push_back("goog-remb");
    HELPER_EXPECT_SUCCESS(env.stream_->initialize(&env.req_, stream_desc.get()));

    HELPER_EXPECT_SUCCESS(env.stream_->send_rtcp_remb());
    ASSERT_EQ(1u, env.receiver_.sent_rtcp_packets_.size());
    EXPECT_EQ(32000, srs_test_remb_bitrate(env.receiver_.sent_rtcp_packets_.back()));
}
