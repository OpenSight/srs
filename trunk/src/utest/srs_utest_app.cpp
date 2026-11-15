//
// Copyright (c) 2013-2025 The SRS Authors
//
// SPDX-License-Identifier: MIT
//
#include <srs_utest_app.hpp>

using namespace std;

#include <srs_kernel_error.hpp>
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
    // audio-only prelude already published a segment. This is not the
    // provisional startup segment: established audio must be preserved.
    HELPER_ASSERT_SUCCESS(muxer.update_video_codec(SrsVideoCodecIdHEVC));
    EXPECT_EQ(SrsVideoCodecIdHEVC, muxer.latest_vcodec());
    EXPECT_EQ(uri, muxer.ts_url());

    std::string unchanged = srs_ut70_read_file(segment);
    EXPECT_EQ(before.size(), unchanged.size());
    EXPECT_TRUE(before.length() == unchanged.length() && !memcmp(before.data(), unchanged.data(), before.length()));

    // The advertised prelude keeps its bytes and its playlist entry.
    std::string prelude_after = srs_ut70_read_file(prelude);
    EXPECT_TRUE(prelude_bytes.length() == prelude_after.length() && !memcmp(prelude_bytes.data(), prelude_after.data(), prelude_bytes.length()));
    EXPECT_TRUE(srs_ut70_read_file(m3u8).find("audio-prelude-0.ts") != std::string::npos);

    // Later video refreshes the PMT to HEVC without deleting prior audio.
    srs_ut70_flush_video(&muxer, &cache, 90000);

    std::string after = srs_ut70_read_file(segment);
    std::vector<std::vector<int> > groups = srs_ut70_pmt_stream_types(after);
    ASSERT_TRUE(groups.size() >= 2);
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(0), SRS_UT70_STREAM_TYPE_H264));
    EXPECT_TRUE(srs_ut70_has_stream_type(groups.at(groups.size() - 1), SRS_UT70_STREAM_TYPE_HEVC));
    EXPECT_TRUE(before.length() <= after.length() && !memcmp(before.data(), after.data(), before.length()));

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

