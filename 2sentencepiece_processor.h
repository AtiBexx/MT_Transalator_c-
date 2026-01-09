// Copyright 2016 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SENTENCEPIECE_PROCESSOR_H_
#define SENTENCEPIECE_PROCESSOR_H_

#include <cstring>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace sentencepiece {
namespace util {

class Status {
 public:
  Status();
  ~Status();
  Status(const Status &);
  Status &operator=(const Status &);
  Status(Status &&) noexcept;
  Status &operator=(Status &&) noexcept;
  bool ok() const;
  const char *message() const;
};

}  // namespace util

class SentencePieceProcessor {
 public:
  SentencePieceProcessor();
  virtual ~SentencePieceProcessor();

  virtual util::Status Load(const std::string &filename);
  virtual util::Status Load(std::istream *in);
  virtual util::Status Load(std::string_view filename);

  virtual util::Status LoadFromSerializedProto(std::string_view serialized);

  virtual util::Status SetEncodeExtraOptions(std::string_view extra_option);

  virtual util::Status SetDecodeExtraOptions(std::string_view extra_option);

  virtual util::Status SetVocabulary(const std::vector<std::string> &valid_vocab);

  virtual util::Status ResetVocabulary();

  virtual util::Status LoadVocabulary(std::string_view filename, int threshold);

  virtual util::Status Encode(std::string_view input,
                              std::vector<std::string> *output) const;

  virtual util::Status Encode(std::string_view input,
                              std::vector<int> *output) const;

  virtual util::Status Decode(const std::vector<std::string> &input,
                              std::string *output) const;

  virtual util::Status Decode(const std::vector<int> &input,
                              std::string *output) const;

  virtual util::Status NBestEncode(std::string_view input, int nbest_size,
                                   std::vector<std::vector<std::string>> *output) const;

  virtual util::Status NBestEncode(std::string_view input, int nbest_size,
                                   std::vector<std::vector<int>> *output) const;

  virtual util::Status SampleEncode(std::string_view input, int nbest_size, float alpha,
                                    std::vector<std::string> *output) const;

  virtual util::Status SampleEncode(std::string_view input, int nbest_size, float alpha,
                                    std::vector<int> *output) const;

  virtual util::Status CalculateEntropy(std::string_view input, float alpha,
                                        float *entropy) const;

  virtual int GetPieceSize() const;

  virtual int PieceToId(std::string_view piece) const;

  virtual const std::string &IdToPiece(int id) const;

  virtual float GetScore(int id) const;

  virtual bool IsUnknown(int id) const;

  virtual bool IsControl(int id) const;

  virtual bool IsUnused(int id) const;

  virtual int GetPieceSize() const; // Duplicate declaration removed in real code

  virtual int unk_id() const;

  virtual int bos_id() const;

  virtual int eos_id() const;

  virtual int pad_id() const;
};

}  // namespace sentencepiece

#endif  // SENTENCEPIECE_PROCESSOR_H_
