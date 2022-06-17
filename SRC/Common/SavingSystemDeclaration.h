#pragma once

#define DECLARE_SAVELOAD_ABILITIES()                                                                                                                                               \
public:                                                                                                                                                                            \
    template<typename Chunk, bool isWriting>                                                                                                                                       \
    void SaveLoad(Chunk&);                                                                                                                                                         \
                                                                                                                                                                                   \
private: