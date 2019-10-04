// Copyright 2019 The Marl Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "marl/conditionvariable.h"

#include "marl_test.h"

TEST_F(WithoutBoundScheduler, ConditionVariable) {
  bool trigger[3] = {false, false, false};
  bool signal[3] = {false, false, false};
  std::mutex mutex;
  marl::ConditionVariable cv;

  std::thread thread([&] {
    for (int i = 0; i < 3; i++) {
      std::unique_lock<std::mutex> lock(mutex);
      cv.wait(lock, [&] { return trigger[i]; });
      signal[i] = true;
      cv.notify_one();
    }
  });

  ASSERT_FALSE(signal[0]);
  ASSERT_FALSE(signal[1]);
  ASSERT_FALSE(signal[2]);

  for (int i = 0; i < 3; i++) {
    {
      std::unique_lock<std::mutex> lock(mutex);
      trigger[i] = true;
      cv.notify_one();
      cv.wait(lock, [&] { return signal[i]; });
    }

    ASSERT_EQ(signal[0], 0 <= i);
    ASSERT_EQ(signal[1], 1 <= i);
    ASSERT_EQ(signal[2], 2 <= i);
  }

  thread.join();
}

TEST_P(WithBoundScheduler, ConditionVariable) {
  bool trigger[3] = {false, false, false};
  bool signal[3] = {false, false, false};
  std::mutex mutex;
  marl::ConditionVariable cv;

  std::thread thread([&] {
    for (int i = 0; i < 3; i++) {
      std::unique_lock<std::mutex> lock(mutex);
      cv.wait(lock, [&] { return trigger[i]; });
      signal[i] = true;
      cv.notify_one();
    }
  });

  ASSERT_FALSE(signal[0]);
  ASSERT_FALSE(signal[1]);
  ASSERT_FALSE(signal[2]);

  for (int i = 0; i < 3; i++) {
    {
      std::unique_lock<std::mutex> lock(mutex);
      trigger[i] = true;
      cv.notify_one();
      cv.wait(lock, [&] { return signal[i]; });
    }

    ASSERT_EQ(signal[0], 0 <= i);
    ASSERT_EQ(signal[1], 1 <= i);
    ASSERT_EQ(signal[2], 2 <= i);
  }

  thread.join();
}
