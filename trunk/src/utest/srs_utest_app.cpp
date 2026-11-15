//
// Copyright (c) 2013-2025 The SRS Authors
//
// SPDX-License-Identifier: MIT
//
#include <srs_utest_app.hpp>

using namespace std;

#include <srs_kernel_error.hpp>
#include <srs_kernel_utility.hpp>
#include <srs_app_fragment.hpp>
#include <srs_app_security.hpp>
#include <srs_app_config.hpp>
#include <srs_app_hls.hpp>
#include <srs_kernel_codec.hpp>
#include <srs_kernel_ts.hpp>
#include <srs_kernel_flv.hpp>
#include <srs_kernel_buffer.hpp>
#include <srs_protocol_rtmp_stack.hpp>
#include <srs_utest_config.hpp>

#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

#include <srs_app_st.hpp>
#include <srs_protocol_conn.hpp>
#include <srs_app_conn.hpp>

class MockIDResource : public ISrsResource
{
public:
    int id;
    MockIDResource(int v) {
        id = v;
    }
    virtual ~MockIDResource() {
    }
    virtual const SrsContextId& get_id() {
        return _srs_context->get_id();
    }
    virtual std::string desc() {
        return "";
    }
};

VOID TEST(AppResourceManagerTest, FindByFastID)
{
    srs_error_t err = srs_success;

    if (true) {
        SrsResourceManager m("test");
        HELPER_EXPECT_SUCCESS(m.start());

        m.add_with_fast_id(101, new MockIDResource(1));
        m.add_with_fast_id(102, new MockIDResource(2));
        m.add_with_fast_id(103, new MockIDResource(3));
        EXPECT_EQ(1, ((MockIDResource*)m.find_by_fast_id(101))->id);
        EXPECT_EQ(2, ((MockIDResource*)m.find_by_fast_id(102))->id);
        EXPECT_EQ(3, ((MockIDResource*)m.find_by_fast_id(103))->id);
    }

    if (true) {
        SrsResourceManager m("test");
        HELPER_EXPECT_SUCCESS(m.start());

        MockIDResource* r1 = new MockIDResource(1);
        MockIDResource* r2 = new MockIDResource(2);
        MockIDResource* r3 = new MockIDResource(3);
        m.add_with_fast_id(101, r1);
        m.add_with_fast_id(102, r2);
        m.add_with_fast_id(103, r3);
        EXPECT_EQ(1, ((MockIDResource*)m.find_by_fast_id(101))->id);
        EXPECT_EQ(2, ((MockIDResource*)m.find_by_fast_id(102))->id);
        EXPECT_EQ(3, ((MockIDResource*)m.find_by_fast_id(103))->id);

        m.remove(r2); srs_usleep(0);
        EXPECT_TRUE(m.find_by_fast_id(102) == NULL);
    }

    if (true) {
        SrsResourceManager m("test");
        HELPER_EXPECT_SUCCESS(m.start());

        MockIDResource* r1 = new MockIDResource(1);
        MockIDResource* r2 = new MockIDResource(2);
        MockIDResource* r3 = new MockIDResource(3);
        m.add_with_fast_id(1, r1);
        m.add_with_fast_id(100001, r2);
        m.add_with_fast_id(1000001, r3);
        EXPECT_EQ(1, ((MockIDResource*)m.find_by_fast_id(1))->id);
        EXPECT_EQ(2, ((MockIDResource*)m.find_by_fast_id(100001))->id);
        EXPECT_EQ(3, ((MockIDResource*)m.find_by_fast_id(1000001))->id);

        m.remove(r2); srs_usleep(0);
        EXPECT_TRUE(m.find_by_fast_id(100001) == NULL);

        m.remove(r3); srs_usleep(0);
        EXPECT_TRUE(m.find_by_fast_id(1000001) == NULL);

        m.remove(r1); srs_usleep(0);
        EXPECT_TRUE(m.find_by_fast_id(1) == NULL);
    }

    if (true) {
        SrsResourceManager m("test");
        HELPER_EXPECT_SUCCESS(m.start());

        m.add_with_fast_id(101, new MockIDResource(1));
        m.add_with_fast_id(10101, new MockIDResource(2));
        m.add_with_fast_id(1010101, new MockIDResource(3));
        m.add_with_fast_id(101010101, new MockIDResource(4));
        m.add_with_fast_id(10101010101LL, new MockIDResource(5));
        m.add_with_fast_id(1010101010101LL, new MockIDResource(6));
        m.add_with_fast_id(101010101010101LL, new MockIDResource(7));
        m.add_with_fast_id(10101010101010101LL, new MockIDResource(8));
        m.add_with_fast_id(1010101010101010101ULL, new MockIDResource(9));
        m.add_with_fast_id(11010101010101010101ULL, new MockIDResource(10));
        EXPECT_EQ(1, ((MockIDResource*)m.find_by_fast_id(101))->id);
        EXPECT_EQ(2, ((MockIDResource*)m.find_by_fast_id(10101))->id);
        EXPECT_EQ(3, ((MockIDResource*)m.find_by_fast_id(1010101))->id);
        EXPECT_EQ(4, ((MockIDResource*)m.find_by_fast_id(101010101))->id);
        EXPECT_EQ(5, ((MockIDResource*)m.find_by_fast_id(10101010101LL))->id);
        EXPECT_EQ(6, ((MockIDResource*)m.find_by_fast_id(1010101010101LL))->id);
        EXPECT_EQ(7, ((MockIDResource*)m.find_by_fast_id(101010101010101LL))->id);
        EXPECT_EQ(8, ((MockIDResource*)m.find_by_fast_id(10101010101010101LL))->id);
        EXPECT_EQ(9, ((MockIDResource*)m.find_by_fast_id(1010101010101010101ULL))->id);
        EXPECT_EQ(10, ((MockIDResource*)m.find_by_fast_id(11010101010101010101ULL))->id);
    }

    if (true) {
        SrsResourceManager m("test");
        HELPER_EXPECT_SUCCESS(m.start());

        m.add_with_fast_id(101, new MockIDResource(1));
        m.add_with_fast_id(101, new MockIDResource(4));
        EXPECT_EQ(1, ((MockIDResource*)m.find_by_fast_id(101))->id);
    }
}

VOID TEST(AppCoroutineTest, Dummy)
{
    SrsDummyCoroutine dc;

    if (true) {
        SrsContextId v = dc.cid();
        EXPECT_TRUE(v.empty());

        srs_error_t err = dc.pull();
        EXPECT_TRUE(err != srs_success);
        EXPECT_TRUE(ERROR_THREAD_DUMMY == srs_error_code(err));
        srs_freep(err);

        err = dc.start();
        EXPECT_TRUE(err != srs_success);
        EXPECT_TRUE(ERROR_THREAD_DUMMY == srs_error_code(err));
        srs_freep(err);
    }

    if (true) {
        dc.stop();

        SrsContextId v = dc.cid();
        EXPECT_TRUE(v.empty());

        srs_error_t err = dc.pull();
        EXPECT_TRUE(err != srs_success);
        EXPECT_TRUE(ERROR_THREAD_DUMMY == srs_error_code(err));
        srs_freep(err);

        err = dc.start();
        EXPECT_TRUE(err != srs_success);
        EXPECT_TRUE(ERROR_THREAD_DUMMY == srs_error_code(err));
        srs_freep(err);
    }

    if (true) {
        dc.interrupt();

        SrsContextId v = dc.cid();
        EXPECT_TRUE(v.empty());

        srs_error_t err = dc.pull();
        EXPECT_TRUE(err != srs_success);
        EXPECT_TRUE(ERROR_THREAD_DUMMY == srs_error_code(err));
        srs_freep(err);

        err = dc.start();
        EXPECT_TRUE(err != srs_success);
        EXPECT_TRUE(ERROR_THREAD_DUMMY == srs_error_code(err));
        srs_freep(err);
    }
}

class MockCoroutineHandler : public ISrsCoroutineHandler {
public:
    SrsSTCoroutine* trd;
    srs_error_t err;
    srs_cond_t running;
    srs_cond_t exited;
    SrsContextId cid;
    // Quit without error.
    bool quit;
public:
    MockCoroutineHandler() : trd(NULL), err(srs_success), quit(false) {
        cid.set_value("0");
        running = srs_cond_new();
        exited = srs_cond_new();
    }
    virtual ~MockCoroutineHandler() {
        srs_cond_destroy(running);
        srs_cond_destroy(exited);
    }
public:
    virtual srs_error_t cycle() {
        srs_error_t r0 = srs_success;

        srs_cond_signal(running);

        // The cid should be generated if empty.
        cid = _srs_context->get_id();

        while (!quit && (r0 = trd->pull()) == srs_success && err == srs_success) {
            srs_usleep(10 * SRS_UTIME_MILLISECONDS);
        }

        srs_cond_signal(exited);

        // The cid might be updated.
        cid = _srs_context->get_id();

        if (err != srs_success) {
            srs_freep(r0);
            return err;
        }

        return r0;
    }
};

VOID TEST(AppCoroutineTest, SetCidOfCoroutine)
{
    srs_error_t err = srs_success;

    MockCoroutineHandler ch;
    SrsSTCoroutine sc("test", &ch);
    ch.trd = &sc;
    EXPECT_TRUE(sc.cid().empty());

    // Start coroutine, which will create the cid.
    HELPER_ASSERT_SUCCESS(sc.start());
    HELPER_ASSERT_SUCCESS(sc.pull());

    srs_cond_timedwait(ch.running, 100 * SRS_UTIME_MILLISECONDS);
    EXPECT_TRUE(!sc.cid().empty());
    EXPECT_TRUE(!ch.cid.empty());

    // Should be a new cid.
    SrsContextId cid = _srs_context->generate_id();
    EXPECT_TRUE(sc.cid().compare(cid) != 0);
    EXPECT_TRUE(ch.cid.compare(cid) != 0);

    // Set the cid and stop the coroutine.
    sc.set_cid(cid);
    sc.stop();

    // Now the cid should be the new one.
    srs_cond_timedwait(ch.exited, 100 * SRS_UTIME_MILLISECONDS);
    EXPECT_TRUE(sc.cid().compare(cid) == 0);
    EXPECT_TRUE(ch.cid.compare(cid) == 0);
}

VOID TEST(AppCoroutineTest, StartStop)
{
    if (true) {
        MockCoroutineHandler ch;
        SrsSTCoroutine sc("test", &ch);
        ch.trd = &sc;
        EXPECT_TRUE(sc.cid().empty());

        // Thread stop after created.
        sc.stop();

        EXPECT_TRUE(sc.cid().empty());

        srs_error_t err = sc.pull();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(ERROR_THREAD_TERMINATED == srs_error_code(err));
        srs_freep(err);

        // Should never reuse a disposed thread.
        err = sc.start();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(ERROR_THREAD_DISPOSED == srs_error_code(err));
        srs_freep(err);
    }

    if (true) {
        MockCoroutineHandler ch;
        SrsSTCoroutine sc("test", &ch);
        ch.trd = &sc;
        EXPECT_TRUE(sc.cid().empty());

        EXPECT_TRUE(srs_success == sc.start());
        EXPECT_TRUE(srs_success == sc.pull());

        srs_cond_timedwait(ch.running, 100 * SRS_UTIME_MILLISECONDS);
        EXPECT_TRUE(!sc.cid().empty());

        // Thread stop after started.
        sc.stop();

        srs_error_t err = sc.pull();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(ERROR_THREAD_INTERRUPED == srs_error_code(err));
        srs_freep(err);

        // Should never reuse a disposed thread.
        err = sc.start();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(ERROR_THREAD_DISPOSED == srs_error_code(err));
        srs_freep(err);
    }

    if (true) {
        MockCoroutineHandler ch;
        SrsSTCoroutine sc("test", &ch);
        ch.trd = &sc;
        EXPECT_TRUE(sc.cid().empty());

        EXPECT_TRUE(srs_success == sc.start());
        EXPECT_TRUE(srs_success == sc.pull());

        // Error when start multiple times.
        srs_error_t err = sc.start();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(ERROR_THREAD_STARTED == srs_error_code(err));
        srs_freep(err);

        err = sc.pull();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(ERROR_THREAD_STARTED == srs_error_code(err));
        srs_freep(err);
    }
}

VOID TEST(AppCoroutineTest, Cycle)
{
    if (true) {
        MockCoroutineHandler ch;
        SrsSTCoroutine sc("test", &ch);
        ch.trd = &sc;

        EXPECT_TRUE(srs_success == sc.start());
        EXPECT_TRUE(srs_success == sc.pull());

        // Set cycle to error.
        ch.err = srs_error_new(-1, "cycle");

        srs_cond_timedwait(ch.running, 100 * SRS_UTIME_MILLISECONDS);

        // The cycle error should be pulled.
        srs_error_t err = sc.pull();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(-1 == srs_error_code(err));
        srs_freep(err);
    }

    if (true) {
        MockCoroutineHandler ch;
        SrsContextId cid;
        SrsSTCoroutine sc("test", &ch, cid.set_value("250"));
        ch.trd = &sc;
        EXPECT_TRUE(!sc.cid().compare(cid));

        EXPECT_TRUE(srs_success == sc.start());
        EXPECT_TRUE(srs_success == sc.pull());

        // After running, the cid in cycle should equal to the thread.
        srs_cond_timedwait(ch.running, 100 * SRS_UTIME_MILLISECONDS);
        EXPECT_TRUE(!ch.cid.compare(cid));
    }

    if (true) {
        MockCoroutineHandler ch;
        SrsSTCoroutine sc("test", &ch);
        ch.trd = &sc;

        EXPECT_TRUE(srs_success == sc.start());
        EXPECT_TRUE(srs_success == sc.pull());

        srs_cond_timedwait(ch.running, 100 * SRS_UTIME_MILLISECONDS);

        // Interrupt thread, set err to interrupted.
        sc.interrupt();

        // Set cycle to error.
        ch.err = srs_error_new(-1, "cycle");

        // When thread terminated, thread will get its error.
        srs_cond_timedwait(ch.exited, 100 * SRS_UTIME_MILLISECONDS);

        // Override the error by cycle error.
        sc.stop();

        // Should be cycle error.
        srs_error_t err = sc.pull();
        EXPECT_TRUE(srs_success != err);
        EXPECT_TRUE(-1 == srs_error_code(err));
        srs_freep(err);
    }

    if (true) {
        MockCoroutineHandler ch;
        SrsSTCoroutine sc("test", &ch);
        ch.trd = &sc;

        EXPECT_TRUE(srs_success == sc.start());
        EXPECT_TRUE(srs_success == sc.pull());

        // Quit without error.
        ch.quit = true;

        // Wait for thread to done.
        srs_cond_timedwait(ch.exited, 100 * SRS_UTIME_MILLISECONDS);

        // Override the error by cycle error.
        sc.stop();

        // Should be cycle error.
        srs_error_t err = sc.pull();
        EXPECT_TRUE(srs_success == err);
        srs_freep(err);
    }
}

void* mock_st_thread_create(void *(*/*start*/)(void *arg), void */*arg*/, int /*joinable*/, int /*stack_size*/) {
    return NULL;
}

VOID TEST(AppCoroutineTest, StartThread)
{
    MockCoroutineHandler ch;
    SrsSTCoroutine sc("test", &ch);
    ch.trd = &sc;

    _ST_THREAD_CREATE_PFN ov = _pfn_st_thread_create;
    _pfn_st_thread_create = (_ST_THREAD_CREATE_PFN)mock_st_thread_create;

    srs_error_t err = sc.start();
    _pfn_st_thread_create = ov;

    EXPECT_TRUE(srs_success != err);
    EXPECT_TRUE(ERROR_ST_CREATE_CYCLE_THREAD == srs_error_code(err));
    srs_freep(err);
}

VOID TEST(AppFragmentTest, CheckDuration)
{
	if (true) {
		SrsFragment frg;
		EXPECT_EQ(-1, frg.start_dts);
		EXPECT_EQ(0, frg.dur);
		EXPECT_FALSE(frg.sequence_header);
	}

	if (true) {
		SrsFragment frg;

		frg.append(0);
		EXPECT_EQ(0, frg.duration());

		frg.append(10);
		EXPECT_EQ(10 * SRS_UTIME_MILLISECONDS, frg.duration());

		frg.append(99);
		EXPECT_EQ(99 * SRS_UTIME_MILLISECONDS, frg.duration());

		frg.append(0x7fffffffLL);
		EXPECT_EQ(0x7fffffffLL * SRS_UTIME_MILLISECONDS, frg.duration());

		frg.append(0xffffffffLL);
		EXPECT_EQ(0xffffffffLL * SRS_UTIME_MILLISECONDS, frg.duration());

		frg.append(0x20c49ba5e353f7LL);
		EXPECT_EQ(0x20c49ba5e353f7LL * SRS_UTIME_MILLISECONDS, frg.duration());
	}

	if (true) {
		SrsFragment frg;

		frg.append(0);
		EXPECT_EQ(0, frg.duration());

		frg.append(0x7fffffffffffffffLL);
		EXPECT_EQ(0, frg.duration());
	}

	if (true) {
		SrsFragment frg;

		frg.append(100);
		EXPECT_EQ(0, frg.duration());

		frg.append(10);
		EXPECT_EQ(0, frg.duration());

		frg.append(100);
		EXPECT_EQ(90 * SRS_UTIME_MILLISECONDS, frg.duration());
	}

	if (true) {
		SrsFragment frg;

		frg.append(-10);
		EXPECT_EQ(0, frg.duration());

		frg.append(-5);
		EXPECT_EQ(0, frg.duration());

		frg.append(10);
		EXPECT_EQ(10 * SRS_UTIME_MILLISECONDS, frg.duration());
	}
}

VOID TEST(AppSecurity, CheckSecurity)
{
    srs_error_t err;

    // Deny if no rules.
    if (true) {
        SrsSecurity sec; SrsRequest rr;
        HELPER_EXPECT_FAILED(sec.do_check(NULL, SrsRtmpConnUnknown, "", &rr));
    }

    // Deny if not allowed.
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnUnknown, "", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("others"); rules.get_or_create("any");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnUnknown, "", &rr));
    }

    // Deny by rule.
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "all");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnPlay, "", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "11.12.13.14");
        if (true) {
            SrsConfDirective* d = new SrsConfDirective();
            d->name = "deny";
            d->args.push_back("play");
            d->args.push_back("12.13.14.15");
            rules.directives.push_back(d);
        }
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "all");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPlay, "", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "11.12.13.14");
        if (true) {
            SrsConfDirective* d = new SrsConfDirective();
            d->name = "deny";
            d->args.push_back("play");
            d->args.push_back("12.13.14.15");
            rules.directives.push_back(d);
        }
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnFMLEPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnFlashPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "all");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnFlashPublish, "11.12.13.14", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnHaivisionPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "all");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPublish, "11.12.13.14", &rr));
    }

    // Allowed if not denied.
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnFMLEPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnFMLEPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnPlay, "11.12.13.14", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnUnknown, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnFlashPublish, "11.12.13.14", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPlay, "11.12.13.14", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "publish", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPublish, "12.13.14.15", &rr));
    }

    // Allowed by rule.
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtcConnPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnFMLEPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnFlashPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnHaivisionPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnHaivisionPublish, "12.13.14.15", &rr));
    }

    // Allowed if not denied.
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "12.13.14.15");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnFMLEPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("deny", "play", "all");
        HELPER_EXPECT_SUCCESS(sec.do_check(&rules, SrsRtmpConnFMLEPublish, "12.13.14.15", &rr));
    }

    // Denied if not allowd.
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "11.12.13.14");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnFMLEPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "11.12.13.14");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "11.12.13.14");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "12.13.14.15");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPlay, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "11.12.13.14");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnHaivisionPublish, "12.13.14.15", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "publish", "11.12.13.14");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnUnknown, "11.12.13.14", &rr));
    }

    // Denied if dup.
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "11.12.13.14");
        rules.get_or_create("deny", "play", "11.12.13.14");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtmpConnPlay, "11.12.13.14", &rr));
    }
    if (true) {
        SrsSecurity sec; SrsRequest rr; SrsConfDirective rules;
        rules.get_or_create("allow", "play", "11.12.13.14");
        rules.get_or_create("deny", "play", "11.12.13.14");
        HELPER_EXPECT_FAILED(sec.do_check(&rules, SrsRtcConnPlay, "11.12.13.14", &rr));
    }

    // SRS apply the following simple strategies one by one:
    //       1. allow all if security disabled.
    //       2. default to deny all when security enabled.
    //       3. allow if matches allow strategy.
    //       4. deny if matches deny strategy.
}


// Issue #70: the first HLS segment must advertise the real video codec. Audio
// written before the codec is discovered is preserved by reopening the segment
// with the real codec and replaying it, and a later codec change keeps the
// media that was already written.

#define SRS_UT70_TS_PMT_PID 0x1001
#define SRS_UT70_TS_VIDEO_PID 0x100
#define SRS_UT70_TS_AUDIO_AAC_PID 0x101
#define SRS_UT70_TS_AUDIO_MP3_PID 0x102
#define SRS_UT70_STREAM_TYPE_H264 0x1B
#define SRS_UT70_STREAM_TYPE_HEVC 0x24
#define SRS_UT70_STREAM_TYPE_AAC 0x0F
#define SRS_UT70_STREAM_TYPE_MP3 0x03
#define SRS_UT70_PAYLOAD_SIZE 16384

// Restore the global config even when a fatal ASSERT_* exits the test body.
class SrsUt70ConfigGuard
{
private:
    SrsConfig* previous_;
public:
    explicit SrsUt70ConfigGuard(SrsConfig* config) : previous_(_srs_config) {
        _srs_config = config;
    }
    virtual ~SrsUt70ConfigGuard() {
        _srs_config = previous_;
    }
};

static std::string srs_ut70_read_file(const std::string& path)
{
    std::ifstream in(path.c_str(), std::ios::binary);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

// The advertised entries of a playlist, in order, with their EXTINF duration.
static std::vector<std::pair<double, std::string> > srs_ut70_m3u8_entries(const std::string& m3u8)
{
    std::vector<std::pair<double, std::string> > entries;

    std::stringstream ss(m3u8);
    std::string line;
    double duration = 0;
    while (std::getline(ss, line)) {
        if (line.compare(0, 8, "#EXTINF:") == 0) {
            duration = ::atof(line.substr(8).c_str());
        } else if (!line.empty() && line.at(0) != '#') {
            entries.push_back(std::make_pair(duration, line));
            duration = 0;
        }
    }

    return entries;
}

static std::vector<std::vector<int> > srs_ut70_pmt_stream_types(const std::string& data)
{
    std::vector<std::vector<int> > groups;
    std::vector<unsigned char> section;

    for (size_t offset = 0; offset + SRS_TS_PACKET_SIZE <= data.size(); offset += SRS_TS_PACKET_SIZE) {
        const unsigned char* packet = (const unsigned char*)data.data() + offset;
        if (packet[0] != 0x47) continue;

        int pid = ((packet[1] & 0x1F) << 8) | packet[2];
        if (pid != SRS_UT70_TS_PMT_PID) continue;

        bool payload_start = (packet[1] & 0x40) != 0;
        int adaptation = (packet[3] >> 4) & 0x03;
        int index = 4;
        if (adaptation & 0x02) {
            index += 1 + packet[4];
        }
        if (!(adaptation & 0x01) || index >= SRS_TS_PACKET_SIZE) continue;

        if (payload_start) {
            int pointer = packet[index];
            int begin = index + 1 + pointer;
            if (begin > SRS_TS_PACKET_SIZE) continue;
            section.assign(packet + begin, packet + SRS_TS_PACKET_SIZE);
        } else if (!section.empty()) {
            section.insert(section.end(), packet + index, packet + SRS_TS_PACKET_SIZE);
        }

        if (section.size() < 12 || section[0] != 0x02) continue;
        int section_length = ((section[1] & 0x0F) << 8) | section[2];
        if ((int)section.size() < 3 + section_length) continue;

        int program_info_length = ((section[10] & 0x0F) << 8) | section[11];
        int position = 12 + program_info_length;
        int end = 3 + section_length - 4;
        std::vector<int> types;
        while (position + 5 <= end) {
            types.push_back(section[position]);
            int es_info_length = ((section[position + 3] & 0x0F) << 8) | section[position + 4];
            position += 5 + es_info_length;
        }
        groups.push_back(types);
        section.clear();
    }

    return groups;
}

static bool srs_ut70_has_stream_type(const std::vector<int>& types, int stream_type)
{
    for (size_t i = 0; i < types.size(); i++) {
        if (types.at(i) == stream_type) return true;
    }
    return false;
}

// Find the first PES packet start for pid and decode its DTS (90kHz). Returns
// the byte offset of that TS packet, or -1 when the segment has no such PES.
static int srs_ut70_first_pes_dts(const std::string& data, int pid, int64_t* dts)
{
    for (size_t offset = 0; offset + SRS_TS_PACKET_SIZE <= data.size(); offset += SRS_TS_PACKET_SIZE) {
        const unsigned char* packet = (const unsigned char*)data.data() + offset;
        if (packet[0] != 0x47) continue;

        int packet_pid = ((packet[1] & 0x1F) << 8) | packet[2];
        if (packet_pid != pid) continue;
        if (!(packet[1] & 0x40)) continue;

        int adaptation = (packet[3] >> 4) & 0x03;
        int index = 4;
        if (adaptation & 0x02) {
            index += 1 + packet[4];
        }
        if (!(adaptation & 0x01) || index + 9 > SRS_TS_PACKET_SIZE) continue;

        SrsBuffer stream((char*)packet + index, SRS_TS_PACKET_SIZE - index);
        SrsMpegPES pes;
        srs_error_t err = pes.decode(&stream);
        if (err != srs_success) {
            srs_freep(err);
            continue;
        }

        if (dts) *dts = pes.dts;
        return (int)offset;
    }

    return -1;
}

// Whether the data contains a run of value with at least length bytes. TS
// payloads are packed contiguously inside each packet, so a short run of the
// audio fill byte proves the media payload reached the segment.
static bool srs_ut70_contains_run(const std::string& data, unsigned char value, int length)
{
    int run = 0;
    for (size_t i = 0; i < data.size(); i++) {
        if ((unsigned char)data.at(i) == value) {
            if (++run >= length) return true;
        } else {
            run = 0;
        }
    }
    return false;
}

// Count the payload-unit starts of pid, which are the PES packet starts.
static int srs_ut70_count_pes_starts(const std::string& data, int pid)
{
    int count = 0;
    for (size_t offset = 0; offset + SRS_TS_PACKET_SIZE <= data.size(); offset += SRS_TS_PACKET_SIZE) {
        const unsigned char* packet = (const unsigned char*)data.data() + offset;
        if (packet[0] != 0x47) continue;

        int packet_pid = ((packet[1] & 0x1F) << 8) | packet[2];
        if (packet_pid != pid) continue;
        if (packet[1] & 0x40) count++;
    }
    return count;
}

// Build the audio side of a real SrsFormat for SrsHls::on_audio. The format
// owns the codec and frame objects.
static void srs_ut70_initialize_audio_format(SrsFormat* format, SrsAudioCodecId id)
{
    format->acodec = new SrsAudioCodecConfig();
    format->acodec->id = id;
    format->acodec->sound_rate = SrsAudioSampleRate44100;
    if (id == SrsAudioCodecIdAAC) {
        format->acodec->aac_object = SrsAacObjectTypeAacLC;
        format->acodec->aac_sample_rate = 4; // 44100Hz in srs_aac_srates.
        format->acodec->aac_channels = 2;
    }

    format->audio = new SrsAudioFrame();
}

static void srs_ut70_set_audio_frame(SrsFormat* format, SrsAudioAacFrameTrait trait, char* sample, int size)
{
    srs_error_t err = format->audio->initialize(format->acodec);
    srs_freep(err);
    format->audio->aac_packet_type = trait;
    err = format->audio->add_sample(sample, size);
    srs_freep(err);
}

static void srs_ut70_initialize_video_format(SrsFormat* format, SrsVideoCodecId id)
{
    format->vcodec = new SrsVideoCodecConfig();
    format->vcodec->id = id;

    format->video = new SrsVideoFrame();
    srs_error_t err = format->video->initialize(format->vcodec);
    srs_freep(err);
}

// A shared message with the given timestamp, as SrsHls receives from the hub.
static SrsSharedPtrMessage* srs_ut70_message(bool audio, int64_t timestamp, int size)
{
    char* payload = new char[size];
    memset(payload, 0x55, size);

    SrsMessageHeader header;
    if (audio) {
        header.initialize_audio(size, (uint32_t)timestamp, 1);
    } else {
        header.initialize_video(size, (uint32_t)timestamp, 1);
    }

    SrsSharedPtrMessage* msg = new SrsSharedPtrMessage();
    srs_error_t err = msg->create(&header, payload, size);
    if (err != srs_success) {
        srs_freep(err);
        srs_freep(msg);
        return NULL;
    }

    return msg;
}

static void srs_ut70_flush_audio_with(SrsHlsMuxer* muxer, SrsTsMessageCache* cache, int64_t dts,
    char fill, int size)
{
    srs_error_t err = srs_success;

    SrsTsMessage* audio = new SrsTsMessage();
    audio->dts = audio->pts = audio->start_pts = dts;
    audio->write_pcr = false;
    audio->sid = SrsTsPESStreamIdAudioCommon;

    std::string payload(size, fill);
    audio->payload->append(payload.data(), (int)payload.size());

    cache->audio = audio;
    HELPER_ASSERT_SUCCESS(muxer->flush_audio(cache));
}

static void srs_ut70_flush_audio(SrsHlsMuxer* muxer, SrsTsMessageCache* cache, int64_t dts)
{
    srs_ut70_flush_audio_with(muxer, cache, dts, '\0', SRS_UT70_PAYLOAD_SIZE);
}

static void srs_ut70_flush_video(SrsHlsMuxer* muxer, SrsTsMessageCache* cache, int64_t dts)
{
    srs_error_t err = srs_success;

    SrsTsMessage* video = new SrsTsMessage();
    video->dts = video->pts = video->start_pts = dts;
    video->write_pcr = true;
    video->sid = SrsTsPESStreamIdVideoCommon;

    std::string payload(SRS_UT70_PAYLOAD_SIZE, '\0');
    video->payload->append(payload.data(), (int)payload.size());

    cache->video = video;
    HELPER_ASSERT_SUCCESS(muxer->flush_video(cache));
}

static void srs_ut70_configure(SrsHlsMuxer* muxer, SrsRequest* req, const std::string& dir)
{
    srs_error_t err = srs_success;

    HELPER_ASSERT_SUCCESS(muxer->update_config(req, "", dir, "[stream].m3u8", "[stream]-[seq].ts",
        1 * SRS_UTIME_SECONDS, 60 * SRS_UTIME_SECONDS, false, 10.0, false, true, false, 0, "", "", ""));
    HELPER_ASSERT_SUCCESS(muxer->segment_open());
}

// The startup segment is not advertised, but it is not disposable: audio
// written before the video codec is known is preserved by the same rule used
// for later segments. A single aggregate has no measured duration, so
// discovery reopens the segment with the real codec and replays the audio
// before the first video keyframe.
VOID TEST(HlsMuxerTest, StartupUnmeasuredAudioReplaysBeforeFirstVideo)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "startup-replay";

    std::string dir = _srs_tmp_file_prefix + "issue70-startup-replay";
    srs_ut70_configure(&muxer, &req, dir);

    std::string uri = muxer.ts_url();
    std::string segment = dir + "/" + uri + ".tmp";

    // Audio writes the provisional AVC program map before the codec is known.
    srs_ut70_flush_audio_with(&muxer, &cache, 36000, 'S', SRS_UT70_PAYLOAD_SIZE);

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(srs_ut70_read_file(segment));
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    // The sequence header discovers HEVC. The startup segment is reopened in
    // place, not discarded, and its audio is replayed with the original
    // payload and timestamp.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());
    EXPECT_EQ(uri, muxer.ts_url());

    std::string replayed = srs_ut70_read_file(segment);
    ASSERT_TRUE(!replayed.empty());
    groups = srs_ut70_pmt_stream_types(replayed);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    int64_t audio_dts = -1;
    int audio_offset = srs_ut70_first_pes_dts(replayed, SRS_UT70_TS_AUDIO_AAC_PID, &audio_dts);
    ASSERT_GE(audio_offset, 0);
    EXPECT_EQ(36000, audio_dts);
    EXPECT_TRUE(srs_ut70_contains_run(replayed, 'S', 32));

    // The first keyframe follows the replayed audio in the same segment.
    srs_ut70_flush_video(&muxer, &cache, 90000);
    HELPER_ASSERT_SUCCESS(muxer.segment_close());

    std::string published = srs_ut70_read_file(dir + "/" + uri);
    ASSERT_TRUE(!published.empty());
    groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    int64_t video_dts = -1;
    int video_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_VIDEO_PID, &video_dts);
    ASSERT_GE(video_offset, 0);
    audio_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_AUDIO_AAC_PID, &audio_dts);
    ASSERT_GE(audio_offset, 0);
    EXPECT_EQ(36000, audio_dts);
    EXPECT_LT(audio_offset, video_offset);
    EXPECT_EQ(90000, video_dts);
    EXPECT_TRUE(srs_ut70_contains_run(published, 'S', 32));

    std::vector<std::pair<double, std::string> > entries = srs_ut70_m3u8_entries(srs_ut70_read_file(dir + "/startup-replay.m3u8"));
    ASSERT_EQ(1, (int)entries.size());
    EXPECT_EQ(uri, entries.at(0).second);
    EXPECT_GT(entries.at(0).first, 0.0);

    muxer.dispose();
}

// A startup audio-only segment whose timing is already measured is finalized
// as its own positive-duration entry; the first video-bearing segment then
// opens with the real HEVC codec and does not replay the finalized audio.
VOID TEST(HlsMuxerTest, StartupMeasuredAudioIsFinalizedAsPositiveEntry)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "startup-measured";

    std::string dir = _srs_tmp_file_prefix + "issue70-startup-measured";
    srs_ut70_configure(&muxer, &req, dir);

    std::string audio_uri = muxer.ts_url();
    srs_ut70_flush_audio_with(&muxer, &cache, 0, 'A', SRS_UT70_PAYLOAD_SIZE);
    srs_ut70_flush_audio_with(&muxer, &cache, 27000, 'B', SRS_UT70_PAYLOAD_SIZE);

    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    std::string video_uri = muxer.ts_url();
    ASSERT_TRUE(audio_uri != video_uri);

    std::string audio_published = srs_ut70_read_file(dir + "/" + audio_uri);
    ASSERT_TRUE(!audio_published.empty());
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(audio_published);
    ASSERT_TRUE(!groups.empty());
    // A pure-audio prelude entry may still declare the configured/default video
    // codec in its PMT, but it carries no video payload, so no real video is
    // mislabelled.
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(srs_ut70_contains_run(audio_published, 'A', 32));
    EXPECT_TRUE(srs_ut70_contains_run(audio_published, 'B', 32));
    EXPECT_LT(srs_ut70_first_pes_dts(audio_published, SRS_UT70_TS_VIDEO_PID, NULL), 0);

    srs_ut70_flush_video(&muxer, &cache, 90000);
    srs_ut70_flush_video(&muxer, &cache, 270000);
    HELPER_ASSERT_SUCCESS(muxer.segment_close());

    std::string video_published = srs_ut70_read_file(dir + "/" + video_uri);
    ASSERT_TRUE(!video_published.empty());
    groups = srs_ut70_pmt_stream_types(video_published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_EQ(0, srs_ut70_count_pes_starts(video_published, SRS_UT70_TS_AUDIO_AAC_PID));

    std::vector<std::pair<double, std::string> > entries = srs_ut70_m3u8_entries(srs_ut70_read_file(dir + "/startup-measured.m3u8"));
    ASSERT_EQ(2, (int)entries.size());
    EXPECT_EQ(audio_uri, entries.at(0).second);
    EXPECT_EQ(video_uri, entries.at(1).second);
    EXPECT_GT(entries.at(0).first, 0.0);
    EXPECT_GT(entries.at(1).first, 0.0);

    muxer.dispose();
}

VOID TEST(HlsMuxerTest, AdoptDiscoveredCodecBeforeFirstPacket)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "adopt";

    std::string dir = _srs_tmp_file_prefix + "issue70-adopt";
    srs_ut70_configure(&muxer, &req, dir);

    std::string uri = muxer.ts_url();
    std::string segment = dir + "/" + uri + ".tmp";

    // No TS packet was written yet, so no segment restart is needed.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());
    EXPECT_EQ(uri, muxer.ts_url());
    EXPECT_TRUE(srs_ut70_pmt_stream_types(srs_ut70_read_file(segment)).empty());

    srs_ut70_flush_video(&muxer, &cache, 0);

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(srs_ut70_read_file(segment));
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    muxer.dispose();
}

VOID TEST(HlsMuxerTest, LaterCodecChangePreservesEstablishedSegment)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "later";

    std::string dir = _srs_tmp_file_prefix + "issue70-later";
    srs_ut70_configure(&muxer, &req, dir);

    std::string uri = muxer.ts_url();
    std::string segment = dir + "/" + uri + ".tmp";

    // Startup codec is AVC; then real audio and video are written.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdAVC));
    srs_ut70_flush_audio(&muxer, &cache, 0);
    srs_ut70_flush_video(&muxer, &cache, 0);

    std::string before = srs_ut70_read_file(segment);
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(before);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    // A later codec change is not an initial discovery: it must keep the
    // established segment and rely on the regular PMT refresh.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());
    EXPECT_EQ(uri, muxer.ts_url());

    std::string unchanged = srs_ut70_read_file(segment);
    EXPECT_EQ(before.size(), unchanged.size());
    EXPECT_TRUE(before.length() == unchanged.length() && !memcmp(before.data(), unchanged.data(), before.length()));

    srs_ut70_flush_video(&muxer, &cache, 9000);

    std::string after = srs_ut70_read_file(segment);
    groups = srs_ut70_pmt_stream_types(after);
    ASSERT_TRUE(groups.size() >= 2);
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(groups.size() - 1), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(before.length() <= after.length() && !memcmp(before.data(), after.data(), before.length()));

    muxer.dispose();
}

VOID TEST(HlsMuxerTest, LaterCodecChangeAfterDefaultCodecPreservesSegment)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "default-later";

    std::string dir = _srs_tmp_file_prefix + "issue70-default-later";
    srs_ut70_configure(&muxer, &req, dir);

    std::string uri = muxer.ts_url();
    std::string segment = dir + "/" + uri + ".tmp";

    // The stream starts with the default AVC codec and no sequence-header
    // update reaches the muxer, so the first video frames establish the
    // segment without arming any discovery flag.
    srs_ut70_flush_video(&muxer, &cache, 0);

    std::string before = srs_ut70_read_file(segment);
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(before);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    // A later HEVC frame reaches the write_video codec path. The established
    // segment must be preserved.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());
    EXPECT_EQ(uri, muxer.ts_url());

    std::string unchanged = srs_ut70_read_file(segment);
    EXPECT_EQ(before.size(), unchanged.size());
    EXPECT_TRUE(before.length() == unchanged.length() && !memcmp(before.data(), unchanged.data(), before.length()));

    srs_ut70_flush_video(&muxer, &cache, 9000);

    std::string after = srs_ut70_read_file(segment);
    groups = srs_ut70_pmt_stream_types(after);
    ASSERT_TRUE(groups.size() >= 2);
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(groups.size() - 1), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(before.length() <= after.length() && !memcmp(before.data(), after.data(), before.length()));

    muxer.dispose();
}

VOID TEST(HlsMuxerTest, AudioOnlyPreludeKeepsMediaWhenHevcDiscovered)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "audio-prelude";

    std::string dir = _srs_tmp_file_prefix + "issue70-audio-prelude";
    srs_ut70_configure(&muxer, &req, dir);

    // The stream starts as pure audio and the first segment is long enough to
    // be published (not dropped) when the controller reaps it.
    srs_ut70_flush_audio(&muxer, &cache, 0);
    srs_ut70_flush_audio(&muxer, &cache, 27000);

    std::string prelude = dir + "/audio-prelude-0.ts";
    std::string m3u8 = dir + "/audio-prelude.m3u8";

    // Reap exactly as SrsHlsController::reap_segment does for audio overflow.
    HELPER_ASSERT_SUCCESS(muxer.segment_close());
    HELPER_ASSERT_SUCCESS(muxer.segment_open());

    std::string prelude_bytes = srs_ut70_read_file(prelude);
    ASSERT_TRUE(!prelude_bytes.empty());
    ASSERT_TRUE(srs_ut70_read_file(m3u8).find("audio-prelude-0.ts") != std::string::npos);

    // More audio into the next, still unadvertised segment.
    srs_ut70_flush_audio(&muxer, &cache, 36000);
    srs_ut70_flush_audio(&muxer, &cache, 54000);

    std::string uri = muxer.ts_url();
    std::string segment = dir + "/" + uri + ".tmp";
    std::string before = srs_ut70_read_file(segment);
    ASSERT_TRUE(!before.empty());

    // The first HEVC discovery of this publish session arrives after an
    // audio-only prelude already published a segment. The current audio-only
    // segment is finalized as its own advertised media, and a new segment is
    // opened with HEVC before the first video packet.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());

    std::string video_uri = muxer.ts_url();
    EXPECT_TRUE(uri != video_uri);

    std::string audio_published = srs_ut70_read_file(dir + "/" + uri);
    ASSERT_TRUE(!audio_published.empty());
    EXPECT_TRUE(audio_published.length() >= before.length());
    EXPECT_TRUE(!memcmp(audio_published.data(), before.data(), before.length()));

    std::vector<std::vector<int> > audio_groups = srs_ut70_pmt_stream_types(audio_published);
    ASSERT_TRUE(!audio_groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(audio_groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(audio_groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    // The advertised prelude keeps its bytes and its playlist entry, and the
    // finalized audio segment is advertised before the video segment.
    std::string prelude_after = srs_ut70_read_file(prelude);
    EXPECT_TRUE(prelude_bytes.length() == prelude_after.length() && !memcmp(prelude_bytes.data(), prelude_after.data(), prelude_bytes.length()));
    std::string playlist = srs_ut70_read_file(m3u8);
    EXPECT_TRUE(playlist.find("audio-prelude-0.ts") != std::string::npos);
    EXPECT_TRUE(playlist.find(uri) != std::string::npos);

    // The first video packet opens a segment whose initial PMT is HEVC.
    srs_ut70_flush_video(&muxer, &cache, 90000);

    std::string after = srs_ut70_read_file(dir + "/" + video_uri + ".tmp");
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(after);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    muxer.dispose();
}

// Exercise the controller wiring: audio written first, then a real HEVC frame
// reaches write_video, which must route through update_video_codec so the
// reopened startup segment advertises HEVC from its first PMT.
VOID TEST(HlsControllerTest, WriteVideoDiscoversHevcAfterProvisionalAudio)
{
    srs_error_t err = srs_success;

    std::string dir = _srs_tmp_file_prefix + "issue70-controller";
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 1;"
            << "        hls_window 60;"
            << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsController controller;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "controller";

    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    // Audio is flushed before the video codec is known, so the startup
    // segment emits a provisional AVC PMT.
    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3;
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[8000];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));
    HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, 0));

    std::string uri = controller.ts_url();
    ASSERT_TRUE(!uri.empty());
    std::string segment = dir + "/" + uri + ".tmp";

    // The startup segment is still unadvertised and holds a provisional AVC
    // program map with the written audio.
    std::string before = srs_ut70_read_file(segment);
    ASSERT_TRUE(!before.empty());
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(before);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    // A real HEVC frame discovers the codec through the controller path. The
    // startup segment is reopened in place and its audio replayed, not
    // discarded with the provisional program map.
    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdHEVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    // A keyframe large enough to flush the segment writer, so the replayed
    // audio is observable before the segment is published.
    char vbytes[16384];
    memset(vbytes, 0, sizeof(vbytes));
    vbytes[0] = 0x02; vbytes[1] = 0x01; vbytes[2] = (char)0xe0; vbytes[3] = 0x44;
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, 18000));
    EXPECT_EQ(uri, controller.ts_url());

    std::string after_discovery = srs_ut70_read_file(segment);
    ASSERT_TRUE(!after_discovery.empty());
    groups = srs_ut70_pmt_stream_types(after_discovery);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    int64_t audio_dts = -1;
    int audio_offset = srs_ut70_first_pes_dts(after_discovery, SRS_UT70_TS_AUDIO_AAC_PID, &audio_dts);
    ASSERT_GE(audio_offset, 0);
    EXPECT_EQ(0, audio_dts);
    EXPECT_TRUE(srs_ut70_contains_run(after_discovery, 0x55, 32));

    // A second keyframe reaps and publishes this segment under the real codec.
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, 180000));

    // Complete the publish so the buffered TS is closed and renamed, then
    // inspect exactly what the playlist would advertise as this segment.
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    std::string segment_bytes = srs_ut70_read_file(dir + "/" + uri);
    ASSERT_TRUE(!segment_bytes.empty());
    groups = srs_ut70_pmt_stream_types(segment_bytes);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    audio_offset = srs_ut70_first_pes_dts(segment_bytes, SRS_UT70_TS_AUDIO_AAC_PID, &audio_dts);
    int64_t video_dts = -1;
    int video_offset = srs_ut70_first_pes_dts(segment_bytes, SRS_UT70_TS_VIDEO_PID, &video_dts);
    ASSERT_GE(audio_offset, 0);
    ASSERT_GE(video_offset, 0);
    EXPECT_EQ(0, audio_dts);
    EXPECT_LT(audio_offset, video_offset);
    EXPECT_TRUE(srs_ut70_contains_run(segment_bytes, 0x55, 32));

    std::vector<std::pair<double, std::string> > entries = srs_ut70_m3u8_entries(srs_ut70_read_file(dir + "/controller.m3u8"));
    ASSERT_TRUE(entries.size() >= 1);
    EXPECT_EQ(uri, entries.at(0).second);
    EXPECT_GT(entries.at(0).first, 0.0);
}

// H.264 is the configured fallback, so the provisional AVC program map is
// already correct when audio is written before the video codec is known. The
// discovered codec must be adopted without rotating the segment and losing
// that audio.
VOID TEST(HlsControllerTest, WriteVideoDiscoversH264AfterProvisionalAudio)
{
    srs_error_t err = srs_success;

    std::string dir = _srs_tmp_file_prefix + "issue70-controller-h264";
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 1;"
            << "        hls_window 60;"
            << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsController controller;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "controller-h264";

    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    // Enough audio to pass the stdio buffer, so the provisional bytes are
    // observable before any video frame arrives.
    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3;
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[8000];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));
    HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, 0));

    std::string uri = controller.ts_url();
    ASSERT_TRUE(!uri.empty());
    std::string segment = dir + "/" + uri + ".tmp";
    std::string before = srs_ut70_read_file(segment);
    ASSERT_TRUE(!before.empty());

    // The sequence-header path calls update_video_codec directly. The matching
    // configured fallback must be recorded without rotating the segment.
    HELPER_ASSERT_SUCCESS(controller.update_video_codec(SrsVideoCodecIdAVC));
    EXPECT_EQ(uri, controller.ts_url());
    std::string after_header = srs_ut70_read_file(segment);
    EXPECT_TRUE(after_header.length() >= before.length() && !memcmp(after_header.data(), before.data(), before.length()));

    // An IDR NAL unit carries the first real H.264 frame.
    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdAVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    char vbytes[4] = {0x65, (char)0x88, (char)0x84, 0x00};
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, 18000));

    // No correction was needed: the segment and its audio bytes stay put.
    EXPECT_EQ(uri, controller.ts_url());
    std::string after = srs_ut70_read_file(segment);
    EXPECT_TRUE(after.length() >= before.length() && !memcmp(after.data(), before.data(), before.length()));

    // A second keyframe reaps and publishes the startup segment. The published
    // segment must still begin with the audio that was written first.
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    std::string published = srs_ut70_read_file(dir + "/" + uri);
    ASSERT_TRUE(!published.empty());
    EXPECT_TRUE(published.length() >= before.length() && !memcmp(published.data(), before.data(), before.length()));
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
}

// With an HEVC configured fallback the provisional program map is already
// correct for HEVC, so discovery must keep the written media as well.
VOID TEST(HlsMuxerTest, AudioFirstConfiguredHevcDoesNotReopen)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse("listen 1935; vhost __defaultVhost__ { hls { enabled on; hls_vcodec h265; } }"));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "configured-hevc";

    std::string dir = _srs_tmp_file_prefix + "issue70-configured-hevc";
    srs_ut70_configure(&muxer, &req, dir);

    std::string uri = muxer.ts_url();
    std::string segment = dir + "/" + uri + ".tmp";

    srs_ut70_flush_audio(&muxer, &cache, 0);
    std::string before = srs_ut70_read_file(segment);
    ASSERT_TRUE(!before.empty());

    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());
    EXPECT_EQ(uri, muxer.ts_url());

    std::string after = srs_ut70_read_file(segment);
    EXPECT_TRUE(after.length() >= before.length() && !memcmp(after.data(), before.data(), before.length()));

    srs_ut70_flush_video(&muxer, &cache, 90000);
    std::string written = srs_ut70_read_file(segment);
    EXPECT_TRUE(written.length() >= before.length() && !memcmp(written.data(), before.data(), before.length()));
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(written);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    muxer.dispose();
}

// The production reap trigger publishes audio-only prelude segments when audio
// runs before video. When the first HEVC frame is discovered afterwards, the
// current audio-only segment cannot start with the real codec, so it must be
// finalized as its own segment (retaining the audio) and the first segment that
// carries video must open with an HEVC program map.
VOID TEST(HlsControllerTest, AudioOnlyPreludeThenVideoStartsWithHevcPmt)
{
    srs_error_t err = srs_success;

    // Unique per run: this test reads the playlist to detect the reap, so a
    // stale playlist from an earlier run must not be observed.
    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-prelude-controller-" << srs_update_system_time();
    std::string dir = dir_ss.str();
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 1;"
            << "        hls_window 60;"
            << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsController controller;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "prelude-controller";

    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3;
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[1024];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));

    std::string m3u8 = dir + "/prelude-controller.m3u8";

    // Drive audio through the controller until the natural absolute overflow
    // (hls_aof_ratio * hls_fragment) reaps and advertises an audio-only segment.
    int64_t pts = 0;
    int writes = 0;
    for (; writes < 600; writes++) {
        HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, pts));
        pts += 1920; // 21.3ms in the 90kHz TS timescale
        if (srs_ut70_read_file(m3u8).find("#EXTINF") != std::string::npos) {
            break;
        }
    }
    ASSERT_TRUE(writes < 600);

    std::vector<std::pair<double, std::string> > advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(1, (int)advertised.size());
    std::string prelude_uri = advertised.at(0).second;
    std::string prelude = srs_ut70_read_file(dir + "/" + prelude_uri);
    ASSERT_TRUE(!prelude.empty());

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(prelude);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    // More audio lands in the next, not yet advertised segment. Keep it below
    // the 100ms minimum duration so the correction must retain it deliberately
    // instead of relying on the normal segment validity window.
    for (int i = 0; i < 3; i++) {
        HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, pts));
        pts += 1920;
    }

    std::string audio_uri = controller.ts_url();
    EXPECT_TRUE(prelude_uri != audio_uri);
    std::string audio_tmp = dir + "/" + audio_uri + ".tmp";
    std::string audio_before = srs_ut70_read_file(audio_tmp);
    ASSERT_TRUE(!audio_before.empty());
    EXPECT_EQ(1, (int)srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8)).size());

    // The video sequence header discovers HEVC. The audio segment is finalized
    // as its own advertised media and a new segment is opened for video.
    HELPER_ASSERT_SUCCESS(controller.update_video_codec(SrsVideoCodecIdHEVC));

    std::string video_uri = controller.ts_url();
    EXPECT_TRUE(audio_uri != video_uri);

    std::string audio_after = srs_ut70_read_file(dir + "/" + audio_uri);
    ASSERT_TRUE(!audio_after.empty());
    EXPECT_TRUE(audio_after.length() >= audio_before.length());
    EXPECT_TRUE(!memcmp(audio_after.data(), audio_before.data(), audio_before.length()));

    advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(2, (int)advertised.size());
    EXPECT_EQ(audio_uri, advertised.at(1).second);

    groups = srs_ut70_pmt_stream_types(audio_after);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    // The first real HEVC frame is written into the new segment. Its first PMT
    // must already be HEVC: no AVC-program-map segment may carry video.
    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdHEVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));

    int64_t video_dts = pts;
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts));

    EXPECT_EQ(video_uri, controller.ts_url());

    // A later keyframe overflows this segment under the normal wait-keyframe
    // rule and publishes exactly the segment that carries the first video.
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts + 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    std::string published = srs_ut70_read_file(dir + "/" + video_uri);
    ASSERT_TRUE(!published.empty());
    groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    // Playlist order is contiguous and the video segment has a playable
    // duration; the audio prelude media is still advertised before it.
    advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_TRUE(advertised.size() >= 3);
    EXPECT_EQ(prelude_uri, advertised.at(0).second);
    EXPECT_EQ(audio_uri, advertised.at(1).second);
    EXPECT_EQ(video_uri, advertised.at(2).second);
    EXPECT_TRUE(advertised.at(1).first > 0);
    EXPECT_TRUE(advertised.at(2).first >= 1.0);
    for (size_t i = 0; i < advertised.size(); i++) {
        EXPECT_TRUE(advertised.at(i).first > 0);
    }
}

// The natural audio overflow reaps the prelude and flushes the triggering
// audio aggregate into the new segment. Until another audio frame arrives that
// segment has only the aggregate's start DTS, so its measured duration is zero.
// Discovering HEVC on the first real video frame must reopen the segment with
// the real codec, replay the retained audio and keep the first keyframe, with
// no separate zero-duration playlist entry.
VOID TEST(HlsControllerTest, AudioOverflowBoundaryMediaDiscoveryReplaysRetainedAudio)
{
    srs_error_t err = srs_success;

    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-boundary-media-" << srs_update_system_time();
    std::string dir = dir_ss.str();
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 1;"
            << "        hls_window 60;"
            << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsController controller;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "boundary-media";

    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3;
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[1024];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));

    std::string m3u8 = dir + "/boundary-media.m3u8";

    // Write audio until the natural absolute overflow advertises the first
    // audio-only prelude segment. The triggering aggregate is flushed into the
    // new current segment by that same write.
    int64_t pts = 0;
    int writes = 0;
    for (; writes < 600; writes++) {
        HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, pts));
        pts += 1920;
        if (srs_ut70_read_file(m3u8).find("#EXTINF") != std::string::npos) {
            break;
        }
    }
    ASSERT_TRUE(writes < 600);

    std::vector<std::pair<double, std::string> > advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(1, (int)advertised.size());
    std::string prelude_uri = advertised.at(0).second;

    // The new segment holds the flushed aggregate but has not been advertised
    // and has not advanced its duration with a later audio frame.
    std::string retained_uri = controller.ts_url();
    int64_t retained_start = pts - 1920;
    EXPECT_TRUE(prelude_uri != retained_uri);
    ASSERT_EQ(1, (int)srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8)).size());

    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdHEVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));

    // The first real video frame carries the discovery. write_video must
    // update the codec, reopen the retained segment and still write the
    // keyframe into it.
    int64_t video_dts = pts;
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts));
    EXPECT_EQ(retained_uri, controller.ts_url());

    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts + 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(2, (int)advertised.size());
    EXPECT_EQ(prelude_uri, advertised.at(0).second);
    EXPECT_EQ(retained_uri, advertised.at(1).second);
    for (size_t i = 0; i < advertised.size(); i++) {
        EXPECT_TRUE(advertised.at(i).first > 0);
    }
    EXPECT_TRUE(advertised.at(1).first >= 1.0);

    std::string published = srs_ut70_read_file(dir + "/" + retained_uri);
    ASSERT_TRUE(!published.empty());

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    int64_t audio_dts = -1;
    int audio_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_AUDIO_AAC_PID, &audio_dts);
    int64_t published_video_dts = -1;
    int video_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_VIDEO_PID, &published_video_dts);
    ASSERT_GE(audio_offset, 0);
    ASSERT_GE(video_offset, 0);
    EXPECT_EQ(retained_start, audio_dts);
    EXPECT_LT(audio_offset, video_offset);
    EXPECT_TRUE(srs_ut70_contains_run(published, 0x55, 32));
}

// The video DTS relationship to the retained audio segment's start DTS. Audio
// DTS comes from the AAC sample clock in SrsHls::on_audio, while video DTS is
// RTMP timestamp * 90 in SrsHls::on_video, so the caller gives no ordering
// guarantee.
enum SrsUt70BoundaryDtsMode
{
    SrsUt70BoundaryDtsEqual,
    SrsUt70BoundaryDtsEarlier,
    SrsUt70BoundaryDtsSameMillisecond,
    SrsUt70BoundaryDtsLater,
};

struct SrsUt70BoundaryResult
{
    bool valid;
    bool replayed;
    int64_t retained_start_dts;
    int64_t video_dts;
    std::string prelude_uri;
    std::string current_uri;
    std::string video_uri;
    std::vector<std::pair<double, std::string> > entries;
    double prelude_extinf;
    double audio_extinf;
    double video_extinf;
    int audio_bytes;
    std::vector<int> audio_first_pmt;
    std::vector<int> video_first_pmt;
    int audio_pes_offset;
    int video_pes_offset;
    int64_t audio_pes_dts;
    bool playlist_prelude_only;
    SrsUt70BoundaryResult() : valid(false), replayed(false), retained_start_dts(-1), video_dts(-1),
        prelude_extinf(0), audio_extinf(0), video_extinf(0), audio_bytes(0),
        audio_pes_offset(-1), video_pes_offset(-1), audio_pes_dts(-1),
        playlist_prelude_only(false)
    {
    }
};

// Drive the natural audio overflow boundary: write AAC frames until the reap
// advertises the prelude, optionally let more audio frames advance the measured
// duration, learn HEVC through the sequence-header path, then write the first
// real HEVC keyframe at the requested DTS relationship and complete publication.
static void srs_ut70_run_boundary_matrix(const std::string& name, SrsUt70BoundaryDtsMode mode,
    int extra_audio_frames, SrsUt70BoundaryResult& r, bool pure_audio = false)
{
    srs_error_t err = srs_success;

    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-boundary-" << name << "-" << srs_update_system_time();
    std::string dir = dir_ss.str();
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 1;"
            << "        hls_window 60;";
    if (pure_audio) {
        // Disable the configured video codec, so the controller aggregates
        // audio into multi-packet TS messages.
        conf_ss << "        hls_vcodec vn;";
    }
    conf_ss << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsController controller;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "boundary";

    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3;
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[1024];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));

    std::string m3u8 = dir + "/boundary.m3u8";

    // The first audio frame is written at DTS 0; each FLV AAC packet is one
    // frame. The write that first advertises a segment is the natural overflow
    // reap; the aggregate it flushed is the new segment's only media and its
    // start DTS is that write's DTS.
    int64_t retained_start = -1;
    int64_t pts = 0;
    for (int writes = 0; writes < 600 && retained_start < 0; writes++) {
        int64_t dts = pts;
        HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, dts));
        pts += 1920;
        if (srs_ut70_read_file(m3u8).find("#EXTINF") != std::string::npos) {
            retained_start = dts;
        }
    }
    ASSERT_TRUE(retained_start >= 0);
    r.retained_start_dts = retained_start;

    // Before discovery only the prelude is advertised; the current segment
    // holds the flushed triggering aggregate but is not in the playlist.
    std::vector<std::pair<double, std::string> > advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(1, (int)advertised.size());
    r.prelude_uri = advertised.at(0).second;
    r.prelude_extinf = advertised.at(0).first;
    r.current_uri = controller.ts_url();
    r.playlist_prelude_only = r.prelude_uri != r.current_uri;

    // More audio frames may advance the segment's measured duration before the
    // codec is discovered; this is the multi-frame control.
    for (int k = 1; k <= extra_audio_frames; k++) {
        HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, retained_start + 1920 * k));
    }

    int64_t video_dts = retained_start;
    if (mode == SrsUt70BoundaryDtsEarlier) {
        ASSERT_TRUE(retained_start >= 1920);
        video_dts = retained_start - 1920;
    } else if (mode == SrsUt70BoundaryDtsSameMillisecond) {
        // update_duration divides ticks by 90. Pick a positive tick delta that
        // still rounds to the retained segment's start millisecond.
        int64_t remainder = retained_start % 90;
        ASSERT_TRUE(remainder < 89);
        video_dts = retained_start + (89 - remainder);
    } else if (mode == SrsUt70BoundaryDtsLater) {
        video_dts = retained_start + 1920;
    }
    ASSERT_TRUE(video_dts >= 0);
    r.video_dts = video_dts;

    // The sequence header discovers HEVC immediately; there is no video media
    // yet and the header carries no timestamp.
    HELPER_ASSERT_SUCCESS(controller.update_video_codec(SrsVideoCodecIdHEVC));

    // A zero-duration retained segment is reopened in place with the real
    // codec and its retained audio replayed, so no separate audio-only entry is
    // advertised. A segment whose duration is already measured keeps the
    // existing close/open path and is advertised as positive-duration media.
    r.replayed = controller.ts_url() == r.current_uri;
    if (!r.replayed) {
        std::string audio_after = srs_ut70_read_file(dir + "/" + r.current_uri);
        r.audio_bytes = (int)audio_after.size();
        std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(audio_after);
        if (!groups.empty()) {
            r.audio_first_pmt = groups.at(0);
        }
    }

    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdHEVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));

    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts));
    r.video_uri = controller.ts_url();

    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts + 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    std::string published = srs_ut70_read_file(dir + "/" + r.video_uri);
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(published);
    if (!groups.empty()) {
        r.video_first_pmt = groups.at(0);
    }

    // The audio PES and the first video keyframe both belong to the published
    // video-bearing segment only when the retained audio was replayed.
    r.audio_pes_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_AUDIO_AAC_PID, &r.audio_pes_dts);
    int64_t video_pes_dts = 0;
    r.video_pes_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_VIDEO_PID, &video_pes_dts);

    r.entries = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    for (size_t i = 0; i < r.entries.size(); i++) {
        if (r.entries.at(i).second == r.prelude_uri) {
            r.prelude_extinf = r.entries.at(i).first;
        } else if (r.replayed && r.entries.at(i).second == r.video_uri) {
            r.video_extinf = r.entries.at(i).first;
        } else if (!r.replayed && r.entries.at(i).second == r.current_uri) {
            r.audio_extinf = r.entries.at(i).first;
        } else if (r.entries.at(i).second == r.video_uri) {
            r.video_extinf = r.entries.at(i).first;
        }
    }

    r.valid = true;
}

// The common contract of the boundary: the prelude is kept, the playlist has
// no zero-duration entry, and the first video-bearing segment starts with the
// real HEVC program map.
static void srs_ut70_expect_boundary_common(const SrsUt70BoundaryResult& r)
{
    ASSERT_TRUE(r.valid);
    EXPECT_TRUE(r.playlist_prelude_only);
    EXPECT_TRUE(srs_ut70_has_stream_type(r.video_first_pmt, SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(r.video_first_pmt, SRS_UT70_STREAM_TYPE_H264));
    EXPECT_GT(r.prelude_extinf, 0.0);
    EXPECT_GE(r.video_extinf, 1.0);
    for (size_t i = 0; i < r.entries.size(); i++) {
        EXPECT_GT(r.entries.at(i).first, 0.0);
    }
}

// A zero-duration unmeasured audio-only segment is reopened with the
// discovered codec, its audio replayed before the first video keyframe, and
// then advertised as part of the video-bearing segment. The payload and
// timestamps are preserved.
static void srs_ut70_expect_replay_boundary(const SrsUt70BoundaryResult& r)
{
    srs_ut70_expect_boundary_common(r);

    EXPECT_TRUE(r.replayed);
    EXPECT_EQ(r.current_uri, r.video_uri);
    EXPECT_TRUE(srs_ut70_has_stream_type(r.video_first_pmt, SRS_UT70_STREAM_TYPE_AAC));

    ASSERT_TRUE(r.entries.size() >= 2);
    EXPECT_EQ(r.prelude_uri, r.entries.at(0).second);
    EXPECT_EQ(r.video_uri, r.entries.at(1).second);

    EXPECT_GE(r.audio_pes_offset, 0);
    EXPECT_GE(r.video_pes_offset, 0);
    EXPECT_LT(r.audio_pes_offset, r.video_pes_offset);
    EXPECT_EQ(r.retained_start_dts, r.audio_pes_dts);
}

// A retained segment whose duration is already measured keeps its existing
// positive-duration audio-only entry; its audio is not replayed into the video
// segment.
static void srs_ut70_expect_kept_audio_entry(const SrsUt70BoundaryResult& r, double min_audio_extinf)
{
    srs_ut70_expect_boundary_common(r);

    EXPECT_TRUE(!r.replayed);
    EXPECT_GT(r.audio_bytes, 0);
    EXPECT_TRUE(srs_ut70_has_stream_type(r.audio_first_pmt, SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(r.audio_first_pmt, SRS_UT70_STREAM_TYPE_HEVC));

    ASSERT_TRUE(r.entries.size() >= 3);
    EXPECT_EQ(r.prelude_uri, r.entries.at(0).second);
    EXPECT_EQ(r.current_uri, r.entries.at(1).second);
    EXPECT_EQ(r.video_uri, r.entries.at(2).second);
    EXPECT_GT(r.audio_extinf, 0.0);
    EXPECT_GE(r.audio_extinf, min_audio_extinf);

    EXPECT_LT(r.audio_pes_offset, 0);
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsEqualReplaysRetainedAudio)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("equal", SrsUt70BoundaryDtsEqual, 0, r);
    srs_ut70_expect_replay_boundary(r);
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsEarlierReplaysRetainedAudio)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("earlier", SrsUt70BoundaryDtsEarlier, 0, r);
    srs_ut70_expect_replay_boundary(r);
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsSameMillisecondReplaysRetainedAudio)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("same-ms", SrsUt70BoundaryDtsSameMillisecond, 0, r);
    srs_ut70_expect_replay_boundary(r);
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsLaterReplaysRetainedAudio)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("later", SrsUt70BoundaryDtsLater, 0, r);
    srs_ut70_expect_replay_boundary(r);
}

TEST(HlsControllerTest, AudioOverflowBoundaryMultiAudioFrameKeepsPositiveDuration)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("multi-audio", SrsUt70BoundaryDtsEqual, 35, r);
    srs_ut70_expect_kept_audio_entry(r, 0.5);
}

// With the default video codec enabled, audio is flushed packet by packet.
// With the configured video codec disabled (pure audio), the cache aggregates
// several packets into one TS message; the same boundary must replay that
// aggregate into the segment reopened with the real codec.
TEST(HlsControllerTest, AudioOverflowBoundaryPureAudioAggregationReplaysRetainedAudio)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("pure-audio", SrsUt70BoundaryDtsEqual, 0, r, true);
    srs_ut70_expect_replay_boundary(r);
}

// A retained segment may hold more than one written aggregate when the audio
// timing does not advance (or regresses) before the codec is discovered. All
// of them are replayed, in write order, with their original timestamps.
VOID TEST(HlsMuxerTest, RetainedAudioReplaysEveryMessageInOrder)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "replay-order";

    std::string dir = _srs_tmp_file_prefix + "issue70-replay-order";
    srs_ut70_configure(&muxer, &req, dir);

    // Advertise an audio-only prelude, then keep the next segment unwritten
    // with respect to its measured duration.
    srs_ut70_flush_audio(&muxer, &cache, 0);
    srs_ut70_flush_audio(&muxer, &cache, 27000);
    HELPER_ASSERT_SUCCESS(muxer.segment_close());
    HELPER_ASSERT_SUCCESS(muxer.segment_open());

    int64_t start = 36000;
    srs_ut70_flush_audio_with(&muxer, &cache, start, 'A', 1024);
    srs_ut70_flush_audio_with(&muxer, &cache, start, 'B', 1024);

    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    std::string uri = muxer.ts_url();

    srs_ut70_flush_video(&muxer, &cache, start);
    srs_ut70_flush_video(&muxer, &cache, start + 180000);
    HELPER_ASSERT_SUCCESS(muxer.segment_close());

    std::string published = srs_ut70_read_file(dir + "/" + uri);
    ASSERT_TRUE(!published.empty());

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    EXPECT_EQ(2, srs_ut70_count_pes_starts(published, SRS_UT70_TS_AUDIO_AAC_PID));

    int64_t first_dts = -1;
    int audio_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_AUDIO_AAC_PID, &first_dts);
    ASSERT_GE(audio_offset, 0);
    EXPECT_EQ(start, first_dts);

    int64_t video_dts = -1;
    int video_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_VIDEO_PID, &video_dts);
    ASSERT_GE(video_offset, 0);
    EXPECT_LT(audio_offset, video_offset);

    // Both retained messages kept their payload.
    EXPECT_TRUE(srs_ut70_contains_run(published, 'A', 32));
    EXPECT_TRUE(srs_ut70_contains_run(published, 'B', 32));

    muxer.dispose();
}

// Retention is bounded: a source that keeps writing audio into the current
// unadvertised segment without advancing its timing must fail explicitly when
// the bounded replay cache is exhausted, instead of growing memory without
// bound or discarding the media silently.
VOID TEST(HlsMuxerTest, RetainedAudioBoundFailsExplicitly)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "replay-bound";

    std::string dir = _srs_tmp_file_prefix + "issue70-replay-bound";
    srs_ut70_configure(&muxer, &req, dir);

    srs_ut70_flush_audio(&muxer, &cache, 0);
    srs_ut70_flush_audio(&muxer, &cache, 27000);
    HELPER_ASSERT_SUCCESS(muxer.segment_close());
    HELPER_ASSERT_SUCCESS(muxer.segment_open());

    // Fill the replay cache exactly, with a non-advancing timestamp so every
    // message stays retained.
    const int max_bytes = 4 * 1024 * 1024;
    const int payload_size = SRS_UT70_PAYLOAD_SIZE;
    for (int i = 0; i < max_bytes / payload_size; i++) {
        srs_ut70_flush_audio_with(&muxer, &cache, 36000, 'A', payload_size);
    }

    // The next message would exceed the bound.
    SrsTsMessage* audio = new SrsTsMessage();
    audio->dts = audio->pts = audio->start_pts = 36000;
    audio->sid = SrsTsPESStreamIdAudioCommon;
    std::string payload(payload_size, 'B');
    audio->payload->append(payload.data(), (int)payload.size());
    cache.audio = audio;

    err = muxer.flush_audio(&cache);
    ASSERT_TRUE(err != srs_success);
    EXPECT_EQ(ERROR_HLS_METADATA, srs_error_code(err));
    srs_freep(err);

    // The rejected message was never taken by the muxer; the cache still owns
    // it and is responsible for freeing it.
    EXPECT_TRUE(cache.audio != NULL);

    muxer.dispose();
}

// A codec change exactly at the retained boundary leaves the current segment's
// duration unmeasured, but its media is preserved: the segment is reopened
// with the discovered codec, the MP3 audio is replayed with its original
// payload and timestamp, and the first HEVC keyframe follows it.
TEST(HlsControllerTest, CodecChangeAtRetainedBoundaryReplaysRetainedAudio)
{
    srs_error_t err = srs_success;

    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-codec-boundary-" << srs_update_system_time();
    std::string dir = dir_ss.str();
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 0.5;"
            << "        hls_aof_ratio 1;"
            << "        hls_window 60;"
            << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsController controller;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "codec-boundary";
    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 4; // 44100 Hz
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[1024];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));

    std::string m3u8 = dir + "/codec-boundary.m3u8";

    // Write the prelude without reaping. The overflow threshold is the
    // configured hls_aof_ratio * hls_fragment = 500ms.
    int64_t dts = 0;
    for (;;) {
        int64_t next = dts + 1920;
        if (next / 90 >= 500) break;
        HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, dts));
        dts = next;
    }
    EXPECT_TRUE(srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8)).empty());

    // The overflowing packet switches the audio codec to MP3 and reaps the
    // prelude. The MP3 aggregate is flushed into the retained segment with no
    // measured duration.
    SrsAudioCodecConfig mp3;
    mp3.id = SrsAudioCodecIdMP3;
    mp3.sound_rate = SrsAudioSampleRate44100;
    SrsAudioFrame mframe;
    char mbytes[1024];
    memset(mbytes, 0x66, sizeof(mbytes));
    HELPER_ASSERT_SUCCESS(mframe.initialize(&mp3));
    HELPER_ASSERT_SUCCESS(mframe.add_sample(mbytes, (int)sizeof(mbytes)));

    int64_t retained_start = dts + 1920;
    HELPER_ASSERT_SUCCESS(controller.write_audio(&mframe, retained_start));

    std::vector<std::pair<double, std::string> > entries = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(1, (int)entries.size());
    std::string prelude_uri = entries.at(0).second;
    EXPECT_GT(entries.at(0).first, 0.0);

    std::string retained_uri = controller.ts_url();
    EXPECT_TRUE(prelude_uri != retained_uri);

    // The discovery must not fail and must not drop the retained media.
    HELPER_ASSERT_SUCCESS(controller.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(retained_uri, controller.ts_url());

    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdHEVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));

    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, retained_start));
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, retained_start + 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    entries = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_TRUE(entries.size() >= 2);
    EXPECT_EQ(prelude_uri, entries.at(0).second);
    EXPECT_EQ(retained_uri, entries.at(1).second);
    for (size_t i = 0; i < entries.size(); i++) {
        EXPECT_TRUE(entries.at(i).first > 0);
    }
    EXPECT_TRUE(entries.at(1).first >= 1.0);

    std::string published = srs_ut70_read_file(dir + "/" + retained_uri);
    ASSERT_TRUE(!published.empty());

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_MP3));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    int64_t audio_dts = -1;
    int audio_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_AUDIO_MP3_PID, &audio_dts);
    int64_t video_dts = -1;
    int video_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_VIDEO_PID, &video_dts);
    ASSERT_GE(audio_offset, 0);
    ASSERT_GE(video_offset, 0);
    EXPECT_EQ(retained_start, audio_dts);
    EXPECT_LT(audio_offset, video_offset);
    EXPECT_TRUE(srs_ut70_contains_run(published, 0x66, 32));
}

// Exercise the real SrsHls::on_audio/on_video seam, including its own DTS
// computation: the first MP3 packet after a cadence reset triggers the audio
// overflow reap, and the HEVC sequence header discovers the codec in the
// resulting zero-duration window. The retained MP3 audio and the first HEVC
// keyframe must both survive into a segment whose first PMT is MP3+HEVC.
TEST(HlsSeamTest, Mp3CadenceResetBoundaryPreservesAudioAndFirstVideo)
{
    srs_error_t err = srs_success;

    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-seam-" << srs_update_system_time();
    std::string dir = dir_ss.str();
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 1;"
            << "        hls_window 60;"
            << "        hls_dts_directly on;"
            << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "seam";

    SrsHls hls;
    HELPER_ASSERT_SUCCESS(hls.initialize(NULL, &req));
    HELPER_ASSERT_SUCCESS(hls.on_publish());

    SrsFormat format;
    srs_ut70_initialize_audio_format(&format, SrsAudioCodecIdAAC);

    char abytes[1024];
    memset(abytes, 0x55, sizeof(abytes));

    std::string m3u8 = dir + "/seam.m3u8";

    // SrsHls::on_audio turns the FLV timestamp into the TS DTS directly
    // (hls_dts_directly on), so 92 AAC packets at 23ms leave the prelude just
    // below the 2.1s absolute overflow threshold (2093ms). The next packet is
    // the first MP3 packet after a cadence reset and triggers the reap.
    for (int i = 0; i < 92; i++) {
        srs_ut70_set_audio_frame(&format, SrsAudioAacFrameTraitRawData, abytes, (int)sizeof(abytes));
        SrsUniquePtr<SrsSharedPtrMessage> msg(srs_ut70_message(true, 23 * i, 8));
        ASSERT_TRUE(msg.get() != NULL);
        HELPER_ASSERT_SUCCESS(hls.on_audio(msg.get(), &format));
        ASSERT_TRUE(srs_ut70_read_file(m3u8).find("#EXTINF") == std::string::npos);
    }

    // The MP3 packet that reaps the prelude lands in the new segment with no
    // measured duration (the cadence of the previous codec was reset).
    format.acodec->id = SrsAudioCodecIdMP3;
    char mbytes[1024];
    memset(mbytes, 0x66, sizeof(mbytes));
    srs_ut70_set_audio_frame(&format, SrsAudioAacFrameTraitRawData, mbytes, (int)sizeof(mbytes));

    SrsUniquePtr<SrsSharedPtrMessage> mp3(srs_ut70_message(true, 23 * 92, 8));
    ASSERT_TRUE(mp3.get() != NULL);
    HELPER_ASSERT_SUCCESS(hls.on_audio(mp3.get(), &format));

    std::vector<std::pair<double, std::string> > advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(1, (int)advertised.size());
    std::string prelude_uri = advertised.at(0).second;
    EXPECT_GT(advertised.at(0).first, 0.0);

    // The HEVC sequence header discovers the video codec inside the
    // zero-duration window. This is the failure point of the estimation
    // approach: it must preserve the retained audio instead of failing.
    SrsFormat vformat;
    srs_ut70_initialize_video_format(&vformat, SrsVideoCodecIdHEVC);
    vformat.video->frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vformat.video->avc_packet_type = SrsVideoAvcFrameTraitSequenceHeader;

    SrsUniquePtr<SrsSharedPtrMessage> sh(srs_ut70_message(false, 23 * 92, 8));
    ASSERT_TRUE(sh.get() != NULL);
    HELPER_ASSERT_SUCCESS(hls.on_video(sh.get(), &vformat));

    // The first HEVC keyframe follows the replayed audio in the same segment.
    vformat.video->avc_packet_type = SrsVideoAvcFrameTraitNALU;
    char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
    HELPER_ASSERT_SUCCESS(vformat.video->add_sample(vbytes, (int)sizeof(vbytes)));

    SrsUniquePtr<SrsSharedPtrMessage> v1(srs_ut70_message(false, 23 * 92, 16));
    ASSERT_TRUE(v1.get() != NULL);
    HELPER_ASSERT_SUCCESS(hls.on_video(v1.get(), &vformat));

    SrsUniquePtr<SrsSharedPtrMessage> v2(srs_ut70_message(false, 23 * 92 + 2000, 16));
    ASSERT_TRUE(v2.get() != NULL);
    HELPER_ASSERT_SUCCESS(hls.on_video(v2.get(), &vformat));

    hls.on_unpublish();

    advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_TRUE(advertised.size() >= 2);
    EXPECT_EQ(prelude_uri, advertised.at(0).second);
    for (size_t i = 0; i < advertised.size(); i++) {
        EXPECT_TRUE(advertised.at(i).first > 0);
    }
    EXPECT_TRUE(advertised.at(1).first >= 1.0);

    std::string prelude = srs_ut70_read_file(dir + "/" + prelude_uri);
    ASSERT_TRUE(!prelude.empty());
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(prelude);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    std::string published = srs_ut70_read_file(dir + "/" + advertised.at(1).second);
    ASSERT_TRUE(!published.empty());

    groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_MP3));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    // The audio keeps the timestamp assigned by SrsHls::on_audio to the MP3
    // reap packet: the FLV timestamp 23 * 92ms in the 90kHz TS timescale.
    int64_t expected_audio_dts = (int64_t)23 * 92 * 90;
    int64_t audio_dts = -1;
    int audio_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_AUDIO_MP3_PID, &audio_dts);
    int64_t video_dts = -1;
    int video_offset = srs_ut70_first_pes_dts(published, SRS_UT70_TS_VIDEO_PID, &video_dts);
    ASSERT_GE(audio_offset, 0);
    ASSERT_GE(video_offset, 0);
    EXPECT_EQ(expected_audio_dts, audio_dts);
    EXPECT_LT(audio_offset, video_offset);
    EXPECT_TRUE(srs_ut70_contains_run(published, 0x66, 32));
}

// Before the first media frame, the source may publish more than one video
// sequence header, for example an HEVC header followed by an AVC header. The
// retained audio must survive every discovery: the cache is kept until video
// establishes the segment, so a further discovery reopens and replays the same
// audio again, discarding the previous temporary file instead of duplicating
// its PES packets. The first keyframe then lands under the final codec's PMT.
struct SrsUt70DiscoveryResult
{
    std::string prelude_uri;
    std::string retained_uri;
    int64_t retained_start;
    int audio_pes_count;
    int64_t audio_dts;
    int audio_offset;
    int video_offset;
    std::vector<int> first_pmt;
    std::vector<std::pair<double, std::string> > entries;
    std::string published;
    SrsUt70DiscoveryResult() : retained_start(-1), audio_pes_count(0), audio_dts(-1),
        audio_offset(-1), video_offset(-1)
    {
    }
};

static void srs_ut70_run_repeated_discovery(const std::string& name, SrsVideoCodecId first,
    SrsVideoCodecId second, SrsUt70DiscoveryResult& r)
{
    srs_error_t err = srs_success;

    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-discoveries-" << name << "-" << srs_update_system_time();
    std::string dir = dir_ss.str();
    std::stringstream conf_ss;
    conf_ss << "listen 1935;"
            << "vhost __defaultVhost__ {"
            << "    hls {"
            << "        enabled on;"
            << "        hls_path " << dir << ";"
            << "        hls_fragment 1;"
            << "        hls_window 60;"
            << "        hls_ts_file [stream]-[seq].ts;"
            << "        hls_m3u8_file [stream].m3u8;"
            << "    }"
            << "}";
    std::string conf_str = conf_ss.str();

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(conf_str.c_str()));
    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsController controller;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "discoveries";
    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3;
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[1024];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));

    std::string m3u8 = dir + "/discoveries.m3u8";

    // The natural overflow reap advertises the audio-only prelude and flushes
    // the triggering aggregate into the new, unmeasured segment.
    int64_t pts = 0;
    int writes = 0;
    for (; writes < 600; writes++) {
        HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, pts));
        pts += 1920;
        if (srs_ut70_read_file(m3u8).find("#EXTINF") != std::string::npos) {
            break;
        }
    }
    ASSERT_TRUE(writes < 600);
    r.retained_start = pts - 1920;

    std::vector<std::pair<double, std::string> > advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(1, (int)advertised.size());
    r.prelude_uri = advertised.at(0).second;
    r.retained_uri = controller.ts_url();
    EXPECT_TRUE(r.prelude_uri != r.retained_uri);

    // Two discoveries with no media frame in between. Both must keep the
    // current segment and its retained audio.
    HELPER_ASSERT_SUCCESS(controller.update_video_codec(first));
    ASSERT_EQ(r.retained_uri, controller.ts_url());
    HELPER_ASSERT_SUCCESS(controller.update_video_codec(second));
    ASSERT_EQ(r.retained_uri, controller.ts_url());

    // The first keyframe belongs to the final codec.
    SrsVideoCodecConfig vcodec;
    vcodec.id = second;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    if (second == SrsVideoCodecIdAVC) {
        char vbytes[4] = {0x65, (char)0x88, (char)0x84, 0x00};
        HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));
    } else {
        char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
        HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));
    }

    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, r.retained_start));
    ASSERT_EQ(r.retained_uri, controller.ts_url());
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, r.retained_start + 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    r.entries = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    r.published = srs_ut70_read_file(dir + "/" + r.retained_uri);

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(r.published);
    if (!groups.empty()) {
        r.first_pmt = groups.at(0);
    }

    r.audio_pes_count = srs_ut70_count_pes_starts(r.published, SRS_UT70_TS_AUDIO_AAC_PID);
    r.audio_offset = srs_ut70_first_pes_dts(r.published, SRS_UT70_TS_AUDIO_AAC_PID, &r.audio_dts);
    int64_t video_dts = -1;
    r.video_offset = srs_ut70_first_pes_dts(r.published, SRS_UT70_TS_VIDEO_PID, &video_dts);
}

// A second, different codec discovered before the first media frame reopens
// the retained segment again. The original audio is preserved once, and the
// first keyframe is written under the final AVC program map.
TEST(HlsControllerTest, MultipleCodecDiscoveriesBeforeFirstFrameReplayRetainedAudio)
{
    SrsUt70DiscoveryResult r;
    srs_ut70_run_repeated_discovery("hevc-avc", SrsVideoCodecIdHEVC, SrsVideoCodecIdAVC, r);

    ASSERT_TRUE(!r.published.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(r.first_pmt, SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(srs_ut70_has_stream_type(r.first_pmt, SRS_UT70_STREAM_TYPE_H264));
    EXPECT_TRUE(!srs_ut70_has_stream_type(r.first_pmt, SRS_UT70_STREAM_TYPE_HEVC));

    // The audio was replayed once per reopen, never duplicated in the file.
    EXPECT_EQ(1, r.audio_pes_count);
    ASSERT_GE(r.audio_offset, 0);
    ASSERT_GE(r.video_offset, 0);
    EXPECT_EQ(r.retained_start, r.audio_dts);
    EXPECT_LT(r.audio_offset, r.video_offset);
    EXPECT_TRUE(srs_ut70_contains_run(r.published, 0x55, 32));

    ASSERT_TRUE(r.entries.size() >= 2);
    EXPECT_EQ(r.prelude_uri, r.entries.at(0).second);
    EXPECT_EQ(r.retained_uri, r.entries.at(1).second);
    for (size_t i = 0; i < r.entries.size(); i++) {
        EXPECT_GT(r.entries.at(i).first, 0.0);
    }
    EXPECT_GE(r.entries.at(1).first, 1.0);
}

// The same codec header repeated before the first media frame is a no-op: the
// already replayed audio stays in place and is not written twice.
TEST(HlsControllerTest, RepeatedSameCodecHeaderKeepsSingleReplayedAudio)
{
    SrsUt70DiscoveryResult r;
    srs_ut70_run_repeated_discovery("hevc-hevc", SrsVideoCodecIdHEVC, SrsVideoCodecIdHEVC, r);

    ASSERT_TRUE(!r.published.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(r.first_pmt, SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(srs_ut70_has_stream_type(r.first_pmt, SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(r.first_pmt, SRS_UT70_STREAM_TYPE_H264));

    EXPECT_EQ(1, r.audio_pes_count);
    ASSERT_GE(r.audio_offset, 0);
    ASSERT_GE(r.video_offset, 0);
    EXPECT_EQ(r.retained_start, r.audio_dts);
    EXPECT_LT(r.audio_offset, r.video_offset);
    EXPECT_TRUE(srs_ut70_contains_run(r.published, 0x55, 32));

    ASSERT_TRUE(r.entries.size() >= 2);
    EXPECT_EQ(r.prelude_uri, r.entries.at(0).second);
    EXPECT_EQ(r.retained_uri, r.entries.at(1).second);
    for (size_t i = 0; i < r.entries.size(); i++) {
        EXPECT_GT(r.entries.at(i).first, 0.0);
    }
    EXPECT_GE(r.entries.at(1).first, 1.0);
}

// Advancing audio timing measures the current segment's duration, so the
// existing close/open path can finalize it and the replay cache is released.
// A long segment may therefore carry more than the replay bound without being
// retained or refused; the bound applies only to audio whose timing cannot be
// measured, which the non-advancing control below still exercises.
VOID TEST(HlsMuxerTest, AdvancingAudioDoesNotAccumulateReplayCache)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "replay-advancing";

    std::string dir = _srs_tmp_file_prefix + "issue70-replay-advancing";
    HELPER_ASSERT_SUCCESS(muxer.update_config(&req, "", dir, "[stream].m3u8", "[stream]-[seq].ts",
        600 * SRS_UTIME_SECONDS, 3600 * SRS_UTIME_SECONDS, false, 10.0, false, true, false, 0, "", "", ""));
    HELPER_ASSERT_SUCCESS(muxer.segment_open());

    // Advertise an audio-only prelude, then keep the next segment current.
    srs_ut70_flush_audio_with(&muxer, &cache, 0, 'P', 1024);
    srs_ut70_flush_audio_with(&muxer, &cache, 27000, 'Q', 1024);
    std::string prelude_uri = muxer.ts_url();
    HELPER_ASSERT_SUCCESS(muxer.segment_close());
    HELPER_ASSERT_SUCCESS(muxer.segment_open());

    std::string audio_uri = muxer.ts_url();

    // More than the 4MiB replay bound of audio with advancing timestamps. The
    // first message is unmeasured, the second measures the segment, and from
    // then on nothing is retained.
    const int max_bytes = 4 * 1024 * 1024;
    const int payload_size = SRS_UT70_PAYLOAD_SIZE;
    int writes = max_bytes / payload_size + 8;
    int64_t dts = 36000;
    for (int i = 0; i < writes; i++) {
        srs_ut70_flush_audio_with(&muxer, &cache, dts, 'A', payload_size);
        dts += 1000;
    }

    // Discovery finalizes the measured audio-only segment and opens a segment
    // for the video codec.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    std::string video_uri = muxer.ts_url();
    EXPECT_TRUE(audio_uri != video_uri);

    srs_ut70_flush_video(&muxer, &cache, dts);
    srs_ut70_flush_video(&muxer, &cache, dts + 180000);
    HELPER_ASSERT_SUCCESS(muxer.segment_close());

    // The whole audio-only media was kept, beyond the replay bound.
    std::string audio_published = srs_ut70_read_file(dir + "/" + audio_uri);
    ASSERT_TRUE(!audio_published.empty());
    EXPECT_GT((int)audio_published.size(), max_bytes);
    EXPECT_TRUE(srs_ut70_contains_run(audio_published, 'A', 32));
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(audio_published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    // The video segment starts with the real codec and carries no replayed
    // audio: the audio-only segment was finalized by the existing path.
    std::string video_published = srs_ut70_read_file(dir + "/" + video_uri);
    ASSERT_TRUE(!video_published.empty());
    groups = srs_ut70_pmt_stream_types(video_published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_EQ(0, srs_ut70_count_pes_starts(video_published, SRS_UT70_TS_AUDIO_AAC_PID));

    std::vector<std::pair<double, std::string> > entries = srs_ut70_m3u8_entries(srs_ut70_read_file(dir + "/replay-advancing.m3u8"));
    ASSERT_TRUE(entries.size() >= 3);
    EXPECT_EQ(prelude_uri, entries.at(0).second);
    EXPECT_EQ(audio_uri, entries.at(1).second);
    EXPECT_EQ(video_uri, entries.at(2).second);
    for (size_t i = 0; i < entries.size(); i++) {
        EXPECT_GT(entries.at(i).first, 0.0);
    }
    EXPECT_GE(entries.at(2).first, 1.0);

    muxer.dispose();
}

// A timestamp regression can return a measured segment's duration to zero
// after the replay cache was released to finalize it. Reopening then would
// discard the media the cache no longer covers, so the segment is finalized by
// the existing close/open path instead: all written audio stays in its own
// media, the first keyframe opens the next segment with the real codec, and
// the entry's duration is restored from the span of timestamps actually
// written into the segment, never fabricated.
VOID TEST(HlsMuxerTest, RegressedTimingAfterMeasuredDurationKeepsMedia)
{
    srs_error_t err = srs_success;

    MockSrsConfig conf;
    HELPER_ASSERT_SUCCESS(conf.parse(_MIN_OK_CONF));

    SrsUt70ConfigGuard config_guard(&conf);

    SrsHlsMuxer muxer;
    SrsTsMessageCache cache;
    SrsRequest req;
    req.vhost = "__defaultVhost__";
    req.app = "live";
    req.stream = "replay-regressed";

    std::string dir = _srs_tmp_file_prefix + "issue70-replay-regressed";
    srs_ut70_configure(&muxer, &req, dir);

    // Advertise an audio-only prelude, then open the current segment.
    srs_ut70_flush_audio_with(&muxer, &cache, 0, 'P', 1024);
    srs_ut70_flush_audio_with(&muxer, &cache, 27000, 'Q', 1024);
    std::string prelude_uri = muxer.ts_url();
    HELPER_ASSERT_SUCCESS(muxer.segment_close());
    HELPER_ASSERT_SUCCESS(muxer.segment_open());

    std::string audio_uri = muxer.ts_url();

    // Unmeasured, then measured positive (releasing the cache), then a
    // regression below the segment's original start that returns the fragment
    // duration to zero.
    srs_ut70_flush_audio_with(&muxer, &cache, 36000, 'C', 1024);
    srs_ut70_flush_audio_with(&muxer, &cache, 54000, 'D', 1024);
    srs_ut70_flush_audio_with(&muxer, &cache, 9000, 'E', 1024);

    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    std::string video_uri = muxer.ts_url();
    EXPECT_TRUE(audio_uri != video_uri);

    srs_ut70_flush_video(&muxer, &cache, 90000);
    srs_ut70_flush_video(&muxer, &cache, 270000);
    HELPER_ASSERT_SUCCESS(muxer.segment_close());

    // Every written audio message is preserved in the finalized segment.
    std::string audio_published = srs_ut70_read_file(dir + "/" + audio_uri);
    ASSERT_TRUE(!audio_published.empty());
    EXPECT_TRUE(srs_ut70_contains_run(audio_published, 'C', 32));
    EXPECT_TRUE(srs_ut70_contains_run(audio_published, 'D', 32));
    EXPECT_TRUE(srs_ut70_contains_run(audio_published, 'E', 32));
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(audio_published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    // The video segment starts with the real codec and does not replay audio.
    std::string video_published = srs_ut70_read_file(dir + "/" + video_uri);
    ASSERT_TRUE(!video_published.empty());
    groups = srs_ut70_pmt_stream_types(video_published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_EQ(0, srs_ut70_count_pes_starts(video_published, SRS_UT70_TS_AUDIO_AAC_PID));

    // The playlist stays contiguous and the regressed segment still carries a
    // positive, media-derived duration: the written timestamps span from the
    // regressed start (9000/90 = 100ms) to the highest timestamp actually
    // written (54000/90 = 600ms), so the observed span is 500ms. The video
    // segment is playable.
    std::vector<std::pair<double, std::string> > entries = srs_ut70_m3u8_entries(srs_ut70_read_file(dir + "/replay-regressed.m3u8"));
    ASSERT_TRUE(entries.size() >= 3);
    EXPECT_EQ(prelude_uri, entries.at(0).second);
    EXPECT_EQ(audio_uri, entries.at(1).second);
    EXPECT_EQ(video_uri, entries.at(2).second);
    EXPECT_GT(entries.at(0).first, 0.0);
    EXPECT_GT(entries.at(1).first, 0.0);
    EXPECT_NEAR(0.5, entries.at(1).first, 0.001);
    EXPECT_GE(entries.at(2).first, 1.0);

    muxer.dispose();
}
