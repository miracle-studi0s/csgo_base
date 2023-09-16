#include "../main_definitions.hpp"
#include "../security/security.hpp"

#include "modules.hpp"
#include "memory.hpp"
#include "offsets.hpp"
#include "threads.hpp"

void c_thread_pool::store() {
	DEBUG_LOG("  [+] Create threads \n\n");

	allocate_thread_id = reinterpret_cast<allocate_thread_id_fn>(GetProcAddress(reinterpret_cast<HMODULE>(__modules->tier0), CXOR("AllocateThreadID")));
	free_thread_id = reinterpret_cast<free_thread_id_fn>(GetProcAddress(reinterpret_cast<HMODULE>(__modules->tier0), CXOR("FreeThreadID")));

	const auto& num_threads = get_threads_count();

	threads.reserve(num_threads);
	for (int i = 0; i < num_threads; ++i)
		threads.emplace_back(&c_thread_pool::run, this);
}

void c_thread_pool::destroy() {
	quite = true;
	q_cv.notify_all();
	for (int i = 0; i < threads.size(); ++i)
		threads[i].join();
}