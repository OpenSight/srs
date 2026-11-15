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


// Issue #70: the first HLS segment must advertise the real video codec. Only
// the startup segment before the codec is discovered is provisional; a later
// codec change must keep the media that was already written.

#define SRS_UT70_TS_PMT_PID 0x1001
#define SRS_UT70_STREAM_TYPE_H264 0x1B
#define SRS_UT70_STREAM_TYPE_HEVC 0x24
#define SRS_UT70_STREAM_TYPE_AAC 0x0F
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

static void srs_ut70_flush_audio(SrsHlsMuxer* muxer, SrsTsMessageCache* cache, int64_t dts)
{
    srs_error_t err = srs_success;

    SrsTsMessage* audio = new SrsTsMessage();
    audio->dts = audio->pts = audio->start_pts = dts;
    audio->write_pcr = false;
    audio->sid = SrsTsPESStreamIdAudioCommon;

    std::string payload(SRS_UT70_PAYLOAD_SIZE, '\0');
    audio->payload->append(payload.data(), (int)payload.size());

    cache->audio = audio;
    HELPER_ASSERT_SUCCESS(muxer->flush_audio(cache));
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

VOID TEST(HlsMuxerTest, ReopenStartupProvisionalSegmentForHevc)
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
    req.stream = "startup";

    std::string dir = _srs_tmp_file_prefix + "issue70-startup";
    srs_ut70_configure(&muxer, &req, dir);

    std::string uri = muxer.ts_url();
    std::string segment = dir + "/" + uri + ".tmp";

    // Audio writes the provisional AVC program map before the codec is known.
    srs_ut70_flush_audio(&muxer, &cache, 0);

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(srs_ut70_read_file(segment));
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    // The sequence header discovers HEVC after the provisional write. The
    // startup segment must be restarted, not published with the AVC PMT.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());
    EXPECT_EQ(uri, muxer.ts_url());
    EXPECT_TRUE(srs_ut70_pmt_stream_types(srs_ut70_read_file(segment)).empty());

    srs_ut70_flush_video(&muxer, &cache, 0);

    groups = srs_ut70_pmt_stream_types(srs_ut70_read_file(segment));
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

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

    // A later codec change is not the provisional startup case: it must keep
    // the established segment and rely on the regular PMT refresh.
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
    // segment must be preserved: only startup audio is provisional.
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
    char abytes[128];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));
    HELPER_ASSERT_SUCCESS(controller.write_audio(&aframe, 0));

    // A real HEVC frame discovers the codec through the controller path.
    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdHEVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, 18000));

    // The provisional audio segment was rotated, so this is the segment that
    // will be published. A second keyframe reaps it under the real codec.
    std::string uri = controller.ts_url();
    ASSERT_TRUE(!uri.empty());
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, 180000));

    // Complete the publish so the buffered TS is closed and renamed, then
    // inspect exactly what the playlist would advertise as this segment.
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    std::string segment_bytes = srs_ut70_read_file(dir + "/" + uri);
    ASSERT_TRUE(!segment_bytes.empty());
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(segment_bytes);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
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

// The natural absolute overflow reaps the prelude and flushes the triggering
// audio aggregate into the new segment. Until another audio frame arrives that
// segment has only the aggregate's start DTS, so its measured duration is zero.
// Discovering HEVC in that exact window must not advertise an EXTINF of zero:
// the retained audio segment needs a positive, truthful duration and the next
// segment must start with the real codec before the first video packet.
VOID TEST(HlsControllerTest, AudioOverflowBoundaryDiscoveryKeepsPositiveDuration)
{
    srs_error_t err = srs_success;

    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-boundary-controller-" << srs_update_system_time();
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
    req.stream = "boundary-controller";

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

    std::string m3u8 = dir + "/boundary-controller.m3u8";

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
    std::string audio_uri = controller.ts_url();
    EXPECT_TRUE(prelude_uri != audio_uri);
    ASSERT_EQ(1, (int)srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8)).size());

    // The sequence header discovers HEVC immediately, in the zero-duration
    // window, before any further audio frame.
    HELPER_ASSERT_SUCCESS(controller.update_video_codec(SrsVideoCodecIdHEVC));

    SrsVideoCodecConfig vcodec;
    vcodec.id = SrsVideoCodecIdHEVC;

    SrsVideoFrame vframe;
    HELPER_ASSERT_SUCCESS(vframe.initialize(&vcodec));
    vframe.frame_type = SrsVideoAvcFrameTypeKeyFrame;
    vframe.cts = 0;
    vframe.dts = 0;
    char vbytes[4] = {0x02, 0x01, (char)0xe0, 0x44};
    HELPER_ASSERT_SUCCESS(vframe.add_sample(vbytes, (int)sizeof(vbytes)));

    // The first real video frame supplies the segment boundary. The audio
    // segment must be finalized before this frame is written, and the video
    // must go to a new segment whose first PMT is HEVC.
    int64_t video_dts = pts;
    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts));

    std::string video_uri = controller.ts_url();
    EXPECT_TRUE(audio_uri != video_uri);

    std::string audio_after = srs_ut70_read_file(dir + "/" + audio_uri);
    ASSERT_TRUE(!audio_after.empty());

    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(audio_after);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));

    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts + 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    std::string published = srs_ut70_read_file(dir + "/" + video_uri);
    ASSERT_TRUE(!published.empty());
    groups = srs_ut70_pmt_stream_types(published);
    ASSERT_TRUE(!groups.empty());
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));

    // The retained audio segment must have a positive duration that reflects
    // its actual media, and the first video-bearing segment must start after
    // it with HEVC.
    advertised = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_TRUE(advertised.size() >= 3);
    EXPECT_EQ(prelude_uri, advertised.at(0).second);
    EXPECT_EQ(audio_uri, advertised.at(1).second);
    EXPECT_EQ(video_uri, advertised.at(2).second);
    EXPECT_TRUE(advertised.at(1).first > 0);
    EXPECT_TRUE(advertised.at(1).first < 1.0);
    EXPECT_TRUE(advertised.at(2).first >= 1.0);
    for (size_t i = 0; i < advertised.size(); i++) {
        EXPECT_TRUE(advertised.at(i).first > 0);
    }
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
    int64_t retained_start_dts;
    int64_t video_dts;
    std::string prelude_uri;
    std::string audio_uri;
    std::string video_uri;
    std::vector<std::pair<double, std::string> > entries;
    double prelude_extinf;
    double audio_extinf;
    double video_extinf;
    int audio_bytes;
    std::vector<int> audio_first_pmt;
    std::vector<int> video_first_pmt;
    bool playlist_prelude_only;
    SrsUt70BoundaryResult() : valid(false), retained_start_dts(-1), video_dts(-1),
        prelude_extinf(0), audio_extinf(0), video_extinf(0), audio_bytes(0),
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
    r.audio_uri = controller.ts_url();
    r.playlist_prelude_only = r.prelude_uri != r.audio_uri;

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
    ASSERT_TRUE(r.video_uri != r.audio_uri);

    // The retained audio segment is finalized and readable now. It must keep
    // its AAC media and never carry the HEVC program map.
    std::string audio_after = srs_ut70_read_file(dir + "/" + r.audio_uri);
    r.audio_bytes = (int)audio_after.size();
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(audio_after);
    if (!groups.empty()) {
        r.audio_first_pmt = groups.at(0);
    }

    HELPER_ASSERT_SUCCESS(controller.write_video(&vframe, video_dts + 180000));
    HELPER_ASSERT_SUCCESS(controller.on_unpublish());

    std::string published = srs_ut70_read_file(dir + "/" + r.video_uri);
    groups = srs_ut70_pmt_stream_types(published);
    if (!groups.empty()) {
        r.video_first_pmt = groups.at(0);
    }

    r.entries = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    for (size_t i = 0; i < r.entries.size(); i++) {
        if (r.entries.at(i).second == r.prelude_uri) {
            r.prelude_extinf = r.entries.at(i).first;
        } else if (r.entries.at(i).second == r.audio_uri) {
            r.audio_extinf = r.entries.at(i).first;
        } else if (r.entries.at(i).second == r.video_uri) {
            r.video_extinf = r.entries.at(i).first;
        }
    }

    r.valid = true;
}

// The common contract of the boundary matrix: the prelude is retained, the
// finalized audio segment keeps its AAC media, the first video-bearing segment
// starts with the real HEVC program map, and the playlist is ordered and does
// not contain a zero-duration entry.
static void srs_ut70_expect_boundary(const SrsUt70BoundaryResult& r, double min_audio_extinf)
{
    ASSERT_TRUE(r.valid);
    EXPECT_TRUE(r.playlist_prelude_only);
    EXPECT_GT(r.audio_bytes, 0);
    EXPECT_TRUE(srs_ut70_has_stream_type(r.audio_first_pmt, SRS_UT70_STREAM_TYPE_AAC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(r.audio_first_pmt, SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(srs_ut70_has_stream_type(r.video_first_pmt, SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(!srs_ut70_has_stream_type(r.video_first_pmt, SRS_UT70_STREAM_TYPE_H264));

    ASSERT_TRUE(r.entries.size() >= 3);
    EXPECT_EQ(r.prelude_uri, r.entries.at(0).second);
    EXPECT_EQ(r.audio_uri, r.entries.at(1).second);
    EXPECT_EQ(r.video_uri, r.entries.at(2).second);
    EXPECT_GT(r.prelude_extinf, 0.0);
    EXPECT_GE(r.video_extinf, 1.0);

    // The requirement under test: a retained audio segment with actual media
    // must never be advertised with a zero (or fabricated) duration.
    EXPECT_GT(r.audio_extinf, 0.0)
        << "retained_start=" << r.retained_start_dts << " video_dts=" << r.video_dts
        << " audio_extinf=" << r.audio_extinf;
    EXPECT_GE(r.audio_extinf, min_audio_extinf)
        << "retained_start=" << r.retained_start_dts << " video_dts=" << r.video_dts
        << " audio_extinf=" << r.audio_extinf;
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsEqualKeepsPositiveDuration)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("equal", SrsUt70BoundaryDtsEqual, 0, r);
    srs_ut70_expect_boundary(r, 0.0);
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsEarlierKeepsPositiveDuration)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("earlier", SrsUt70BoundaryDtsEarlier, 0, r);
    srs_ut70_expect_boundary(r, 0.0);
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsSameMillisecondKeepsPositiveDuration)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("same-ms", SrsUt70BoundaryDtsSameMillisecond, 0, r);
    srs_ut70_expect_boundary(r, 0.0);
}

TEST(HlsControllerTest, AudioOverflowBoundaryVideoDtsLaterKeepsPositiveDuration)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("later", SrsUt70BoundaryDtsLater, 0, r);
    srs_ut70_expect_boundary(r, 0.0);
}

TEST(HlsControllerTest, AudioOverflowBoundaryMultiAudioFrameKeepsPositiveDuration)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("multi-audio", SrsUt70BoundaryDtsEqual, 35, r);
    srs_ut70_expect_boundary(r, 0.5);
}

// With the default video codec enabled, audio is flushed packet by packet.
// With the configured video codec disabled (pure audio), the cache aggregates
// several packets into one TS message; the same boundary must still be measured
// by the media end of the aggregate actually written.
TEST(HlsControllerTest, AudioOverflowBoundaryPureAudioAggregationKeepsPositiveDuration)
{
    SrsUt70BoundaryResult r;
    srs_ut70_run_boundary_matrix("pure-audio", SrsUt70BoundaryDtsEqual, 0, r, true);
    srs_ut70_expect_boundary(r, 0.0);
}

// The cache owns the media end of an audio aggregate, including the packets
// after the one whose DTS becomes the message start.
TEST(HlsTsCacheTest, AudioAggregateEndUsesLastAacPacket)
{
    srs_error_t err = srs_success;

    SrsTsMessageCache cache;

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3; // 48000 Hz
    acodec.aac_channels = 2;

    SrsAudioFrame frame;
    HELPER_ASSERT_SUCCESS(frame.initialize(&acodec));
    char bytes[128];
    memset(bytes, 0x55, sizeof(bytes));
    HELPER_ASSERT_SUCCESS(frame.add_sample(bytes, (int)sizeof(bytes)));

    HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 0));
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 1920));
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 3840));

    ASSERT_TRUE(cache.audio != NULL);
    // The message keeps the first packet's start and DTS, while its media end
    // is the end of the last packet actually aggregated into it.
    EXPECT_EQ((int64_t)0, cache.audio->start_pts);
    EXPECT_EQ((int64_t)0, cache.audio->dts);
    EXPECT_EQ((int64_t)5760, cache.audio->end_dts);
}

// AAC packet duration comes from the AudioSpecificConfig sampling frequency
// and the number of raw AAC frames, not from the FLV SoundRate tag.
TEST(HlsTsCacheTest, AudioAacEndUsesConfiguredRateAndFrameCount)
{
    srs_error_t err = srs_success;

    char bytes[128];
    memset(bytes, 0x55, sizeof(bytes));

    // Two raw_data_blocks of 1024 samples at 44.1 kHz: 2048*90000/44100 ticks.
    {
        SrsTsMessageCache cache;
        SrsAudioCodecConfig acodec;
        acodec.id = SrsAudioCodecIdAAC;
        acodec.aac_object = SrsAacObjectTypeAacLC;
        acodec.aac_sample_rate = 4; // 44100 Hz
        acodec.sound_rate = SrsAudioSampleRate22050;

        SrsAudioFrame frame;
        HELPER_ASSERT_SUCCESS(frame.initialize(&acodec));
        HELPER_ASSERT_SUCCESS(frame.add_sample(bytes, (int)sizeof(bytes)));
        HELPER_ASSERT_SUCCESS(frame.add_sample(bytes, (int)sizeof(bytes)));
        HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 0));

        ASSERT_TRUE(cache.audio != NULL);
        EXPECT_EQ((int64_t)4179, cache.audio->end_dts);
    }

    // One raw_data_block at 8 kHz.
    {
        SrsTsMessageCache cache;
        SrsAudioCodecConfig acodec;
        acodec.id = SrsAudioCodecIdAAC;
        acodec.aac_object = SrsAacObjectTypeAacLC;
        acodec.aac_sample_rate = 11; // 8000 Hz
        acodec.sound_rate = SrsAudioSampleRate44100;

        SrsAudioFrame frame;
        HELPER_ASSERT_SUCCESS(frame.initialize(&acodec));
        HELPER_ASSERT_SUCCESS(frame.add_sample(bytes, (int)sizeof(bytes)));
        HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 1000));

        ASSERT_TRUE(cache.audio != NULL);
        EXPECT_EQ((int64_t)12520, cache.audio->end_dts);
    }
}

// Profiles and sampling-frequency indexes whose frame length is not modeled
// use the validated packet cadence instead of a guessed 1024 samples.
TEST(HlsTsCacheTest, AudioAacUnsupportedFrameMetadataFallsBackToCadence)
{
    srs_error_t err = srs_success;

    char bytes[128];
    memset(bytes, 0x55, sizeof(bytes));

    // HE-AAC has no supported 1024-sample frame length in this cache.
    {
        SrsTsMessageCache cache;
        SrsAudioCodecConfig acodec;
        acodec.id = SrsAudioCodecIdAAC;
        acodec.aac_object = SrsAacObjectTypeAacHE;
        acodec.aac_sample_rate = 4; // 44100 Hz

        SrsAudioFrame frame;
        HELPER_ASSERT_SUCCESS(frame.initialize(&acodec));
        HELPER_ASSERT_SUCCESS(frame.add_sample(bytes, (int)sizeof(bytes)));
        HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 0));

        ASSERT_TRUE(cache.audio != NULL);
        EXPECT_EQ((int64_t)0, cache.audio->end_dts);

        HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 2090));
        EXPECT_EQ((int64_t)4180, cache.audio->end_dts);
    }

    // An unset sampling-frequency index is not guessed from the FLV tag.
    {
        SrsTsMessageCache cache;
        SrsAudioCodecConfig acodec;
        acodec.id = SrsAudioCodecIdAAC;
        acodec.aac_object = SrsAacObjectTypeAacLC;
        acodec.aac_sample_rate = SrsAacSampleRateUnset;
        acodec.sound_rate = SrsAudioSampleRate44100;

        SrsAudioFrame frame;
        HELPER_ASSERT_SUCCESS(frame.initialize(&acodec));
        HELPER_ASSERT_SUCCESS(frame.add_sample(bytes, (int)sizeof(bytes)));
        HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 0));

        ASSERT_TRUE(cache.audio != NULL);
        EXPECT_EQ((int64_t)0, cache.audio->end_dts);

        HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 1920));
        EXPECT_EQ((int64_t)3840, cache.audio->end_dts);
    }
}

// MP3 has no frame parser here; its packet duration is the validated cadence
// of the same audio clock, and a timestamp discontinuity does not become a
// packet duration.
TEST(HlsTsCacheTest, AudioMp3EndUsesValidatedCadence)
{
    srs_error_t err = srs_success;

    SrsTsMessageCache cache;

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdMP3;
    acodec.sound_rate = SrsAudioSampleRate44100;

    SrsAudioFrame frame;
    HELPER_ASSERT_SUCCESS(frame.initialize(&acodec));
    char bytes[128];
    memset(bytes, 0x55, sizeof(bytes));
    HELPER_ASSERT_SUCCESS(frame.add_sample(bytes, (int)sizeof(bytes)));

    HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 0));
    ASSERT_TRUE(cache.audio != NULL);
    EXPECT_EQ((int64_t)0, cache.audio->end_dts);

    HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 2090));
    EXPECT_EQ((int64_t)4180, cache.audio->end_dts);

    // A backward jump is a discontinuity, not a negative packet duration.
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 1000));
    EXPECT_EQ((int64_t)4180, cache.audio->end_dts);

    // The cadence restarts from the new position.
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&frame, 3090));
    EXPECT_EQ((int64_t)5180, cache.audio->end_dts);
}

// Publish and audio codec changes reset the cadence history.
TEST(HlsTsCacheTest, AudioTimingResetDropsCodecCadence)
{
    srs_error_t err = srs_success;

    SrsTsMessageCache cache;
    char bytes[128];
    memset(bytes, 0x55, sizeof(bytes));

    SrsAudioCodecConfig aac;
    aac.id = SrsAudioCodecIdAAC;
    aac.aac_object = SrsAacObjectTypeAacLC;
    aac.aac_sample_rate = 3; // 48000 Hz
    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&aac));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(bytes, (int)sizeof(bytes)));
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&aframe, 0));

    // The muxer flushes the aggregate, then the audio codec changes.
    srs_freep(cache.audio);
    cache.reset_audio_timing();

    SrsAudioCodecConfig mp3;
    mp3.id = SrsAudioCodecIdMP3;
    mp3.sound_rate = SrsAudioSampleRate44100;
    SrsAudioFrame mframe;
    HELPER_ASSERT_SUCCESS(mframe.initialize(&mp3));
    HELPER_ASSERT_SUCCESS(mframe.add_sample(bytes, (int)sizeof(bytes)));
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&mframe, 1920));

    // Without the reset, the MP3 packet would inherit the AAC packet cadence
    // and get an end of 3840.
    ASSERT_TRUE(cache.audio != NULL);
    EXPECT_EQ((int64_t)0, cache.audio->end_dts);
}

// A rejected or empty frame must not become the media end reference for
// future media, although the audio clock may still advance.
TEST(HlsTsCacheTest, AudioRejectedOrEmptyPacketDoesNotAdvanceMediaEnd)
{
    srs_error_t err = srs_success;

    SrsTsMessageCache cache;

    SrsAudioCodecConfig mp3;
    mp3.id = SrsAudioCodecIdMP3;
    mp3.sound_rate = SrsAudioSampleRate44100;
    SrsAudioFrame mframe;
    char mbytes[128];
    memset(mbytes, 0x55, sizeof(mbytes));
    HELPER_ASSERT_SUCCESS(mframe.initialize(&mp3));
    HELPER_ASSERT_SUCCESS(mframe.add_sample(mbytes, (int)sizeof(mbytes)));
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&mframe, 0));

    // This AAC sample is too large for an ADTS header, so cache_audio rejects
    // it before it can update the packet timing.
    SrsAudioCodecConfig aac;
    aac.id = SrsAudioCodecIdAAC;
    aac.aac_object = SrsAacObjectTypeAacLC;
    aac.aac_sample_rate = 3; // 48000 Hz
    SrsAudioFrame bad;
    char bad_bytes[0x2000];
    memset(bad_bytes, 0x11, sizeof(bad_bytes));
    HELPER_ASSERT_SUCCESS(bad.initialize(&aac));
    HELPER_ASSERT_SUCCESS(bad.add_sample(bad_bytes, (int)sizeof(bad_bytes)));

    err = cache.cache_audio(&bad, 5000);
    ASSERT_TRUE(err != srs_success);
    EXPECT_EQ(ERROR_HLS_AAC_FRAME_LENGTH, srs_error_code(err));
    srs_freep(err);

    // The cadence still comes from the last accepted packet at DTS 0.
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&mframe, 1000));
    ASSERT_TRUE(cache.audio != NULL);
    EXPECT_EQ((int64_t)2000, cache.audio->end_dts);

    // An empty packet advances the clock but contributes no media duration.
    SrsAudioFrame empty;
    HELPER_ASSERT_SUCCESS(empty.initialize(&mp3));
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&empty, 2000));
    EXPECT_EQ((int64_t)2000, cache.audio->end_dts);

    // The next real packet is measured from the empty packet's clock.
    HELPER_ASSERT_SUCCESS(cache.cache_audio(&mframe, 3000));
    EXPECT_EQ((int64_t)4000, cache.audio->end_dts);
}

// A codec change exactly at the retained boundary can leave the written
// aggregate without a derivable media end. The muxer must fail explicitly
// instead of advertising a zero-duration entry or borrowing the video clock.
TEST(HlsControllerTest, CodecChangeAtRetainedBoundaryWithoutAudioEndFailsExplicitly)
{
    srs_error_t err = srs_success;

    std::stringstream dir_ss;
    dir_ss << _srs_tmp_file_prefix << "issue70-no-audio-end-" << srs_update_system_time();
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
    req.stream = "no-audio-end";
    HELPER_ASSERT_SUCCESS(controller.on_publish(&req));

    SrsAudioCodecConfig acodec;
    acodec.id = SrsAudioCodecIdAAC;
    acodec.aac_object = SrsAacObjectTypeAacLC;
    acodec.aac_sample_rate = 3; // 48000 Hz
    acodec.aac_channels = 2;

    SrsAudioFrame aframe;
    HELPER_ASSERT_SUCCESS(aframe.initialize(&acodec));
    char abytes[1024];
    memset(abytes, 0x55, sizeof(abytes));
    HELPER_ASSERT_SUCCESS(aframe.add_sample(abytes, (int)sizeof(abytes)));

    std::string m3u8 = dir + "/no-audio-end.m3u8";

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

    // The overflowing packet switches the audio codec to MP3. The codec change
    // resets the packet cadence, and this cache has no MP3 frame parser, so the
    // aggregate flushed into the retained segment has no media end.
    SrsAudioCodecConfig mp3;
    mp3.id = SrsAudioCodecIdMP3;
    mp3.sound_rate = SrsAudioSampleRate44100;
    SrsAudioFrame mframe;
    char mbytes[1024];
    memset(mbytes, 0x66, sizeof(mbytes));
    HELPER_ASSERT_SUCCESS(mframe.initialize(&mp3));
    HELPER_ASSERT_SUCCESS(mframe.add_sample(mbytes, (int)sizeof(mbytes)));
    HELPER_ASSERT_SUCCESS(controller.write_audio(&mframe, dts + 1920));

    // The prelude was advertised; the retained segment now holds the MP3
    // packet and no measured duration.
    std::vector<std::pair<double, std::string> > entries = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    ASSERT_EQ(1, (int)entries.size());
    EXPECT_GT(entries.at(0).first, 0.0);

    std::string retained = controller.ts_url();
    std::string retained_tmp = dir + "/" + retained + ".tmp";
    {
        std::ifstream retained_in(retained_tmp.c_str(), std::ios::binary);
        EXPECT_TRUE(retained_in.good());
    }

    err = controller.update_video_codec(SrsVideoCodecIdHEVC);
    ASSERT_TRUE(err != srs_success);
    EXPECT_EQ(ERROR_HLS_METADATA, srs_error_code(err));
    srs_freep(err);

    // No zero-duration entry was published, and the unadvertised segment was
    // not replaced by a fabricated duration; the explicit error is the policy.
    entries = srs_ut70_m3u8_entries(srs_ut70_read_file(m3u8));
    EXPECT_EQ(1, (int)entries.size());
    EXPECT_TRUE(!retained.empty() && entries.at(0).second != retained);

    HELPER_ASSERT_SUCCESS(controller.on_unpublish());
}

