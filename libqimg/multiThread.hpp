
//  Copyright 2021 Isoheptane
//  Filename    : mutliThread.hpp
//  Purpose     : Support Mulit Thread Tasks
//  License     : MIT License

#ifndef _LIBQIMG_MTHREAD_HPP_
#define _LIBQIMG_MTHREAD_HPP_

#include <iostream>
#include <cstring>
#include <pthread.h>
#include <thread>

#include "libqimg_debuglog.hpp"
#include "fmat.hpp"
#include "fmc.hpp"

namespace libqimg::MultiThread {

    int defaultThreadCount = 8;

    /*
        FMC TaskBlock MultiThread
    */
    
    template <class Function>
    struct FMCTaskBlock {

        FMC* collection;
        Point begin, end;
        Function* function;
        std::string taskName;
        int threadId;
        bool* completed;
        
        FMCTaskBlock(
            FMC* collection, 
            Point begin, 
            Point end, 
            Function* function, 
            std::string taskName, 
            int tid,
            bool* completePtr
        ):  collection(collection), 
            begin(begin), 
            end(end), 
            function(function), 
            taskName(taskName), 
            threadId(tid),
            completed(completePtr) {}

    };

    #define FMC_CANVAS_MTEXEC_PARAMS FMC_CANVAS_FOREACH_PARAMS

    // Execute a single FMCTaskBlock
    template <class Function> void* executeFMCTaskBlockCanvas(void* importArgs) {
        FMCTaskBlock<Function>* taskBlock = (FMCTaskBlock<Function>*) importArgs;
#ifdef LIBQIMG_SHOWLOG    
        printf("[Task \"%s\" #% 3d ] Begin execution.\n", 
            taskBlock->taskName.data(), 
            taskBlock->threadId);
#endif
        taskBlock->collection->canvasForeach(
            *taskBlock->function,
            taskBlock->begin, 
            taskBlock->end
        );
        *taskBlock->completed = true;
#ifdef LIBQIMG_SHOWLOG    
        printf("[Task \"%s\" #% 3d ]  -> Execution completed.\n",
            taskBlock->taskName.data(), 
            taskBlock->threadId);
#endif
        delete taskBlock;
        pthread_exit(NULL);
    }

    #define FMC_CHANNEL_MTEXEC_PARAMS FMC_CHANNEL_FOREACH_PARAMS

    // Execute a single FMCTaskBlock
    template <class Function> void* executeFMCTaskBlockChannel(void* importArgs) {
        FMCTaskBlock<Function>* taskBlock = (FMCTaskBlock<Function>*) importArgs;
#ifdef LIBQIMG_SHOWLOG    
        printf("[Task \"%s\" #% 3d ] Begin execution.\n", 
            taskBlock->taskName.data(), 
            taskBlock->threadId);
#endif
        taskBlock->collection->channelForeach(
            *taskBlock->function,
            taskBlock->begin, 
            taskBlock->end
        );
        *taskBlock->completed = true;
#ifdef LIBQIMG_SHOWLOG    
        printf("[Task \"%s\" #% 3d ]  -> Execution completed.\n",
            taskBlock->taskName.data(), 
            taskBlock->threadId);
#endif
        delete taskBlock;
        pthread_exit(NULL);
    }

    /*
        FMAT TaskBlock MultiThread
    */

    template <class Function>
    struct FMATTaskBlock {

        FMAT* matrix;
        Point begin, end;
        Function* function;
        std::string taskName;
        int threadId;
        bool* completed;
        
        FMATTaskBlock(
            FMAT* matrix, 
            Point begin, 
            Point end, 
            Function* function, 
            std::string taskName, 
            int tid,
            bool* completePtr
        ):  matrix(matrix), 
            begin(begin), 
            end(end), 
            function(function), 
            taskName(taskName), 
            threadId(tid),
            completed(completePtr) {}

    };

    #define FMAT_MTEXEC_PARAMS FMAT_PARAMETERIZED_FOREACH_PARAMS

    // Execute a single FMATTaskBlock
    template <class Function> void* executeFMATTaskBlock(void* importArgs) {
        FMATTaskBlock<Function>* taskBlock = (FMATTaskBlock<Function>*) importArgs;
#ifdef LIBQIMG_SHOWLOG    
        printf("[Task \"%s\" #% 3d ] Begin execution.\n", 
            taskBlock->taskName.data(), 
            taskBlock->threadId);
#endif
        taskBlock->matrix->parameterizedForeach(
            *taskBlock->function, 
            taskBlock->begin, 
            taskBlock->end
        );
        *taskBlock->completed = true;
#ifdef LIBQIMG_SHOWLOG    
        printf("[Task \"%s\" #% 3d ]  -> Execution completed.\n",
            taskBlock->taskName.data(), 
            taskBlock->threadId);
#endif
        delete taskBlock;
        pthread_exit(NULL);
    }

    /*
        mtFunction
    */

    #define MTEXEC_PARAMS int threadCount = MultiThread::defaultThreadCount, std::string taskName = "$anonymous"

    // Execute function on multi cores, may highly improve performance.
    template <class Function> bool multiThreadExecuteCanvas(
        FMC& collection, 
        Function function, 
        std::string taskName = "$anonymous", 
        int threadCount = defaultThreadCount
    ) {

        // Check if able to chunk
        if(collection.width() < threadCount || collection.height() < threadCount) {
#ifdef LIBQIMG_SHOWLOG    
            printf("[Task \"%s\" #### ] Denied execution. Collection size too small.\n", 
                taskName.data());
#endif      
            return false;
        }
        pthread_t threads[threadCount];
        bool completed[threadCount];
        for(int i = 0; i < threadCount; i++)
            completed[i] = false;
        int cur = 0;
        int step = collection.height() / threadCount;
        // Create threads
        for(int i = 0; i < threadCount; i++) {
            if(i != threadCount - 1) {
                pthread_create(&threads[i], NULL, executeFMCTaskBlockCanvas<Function>, 
                    new FMCTaskBlock<Function>(&collection,
                        Point(0, cur), Point(collection.width() - 1, cur + step - 1),
                        &function, taskName, i, &completed[i]
                    ));
            } else {
                pthread_create(&threads[i], NULL, executeFMCTaskBlockCanvas<Function>, 
                    new FMCTaskBlock<Function>(&collection,
                        Point(0, cur), collection.end(),
                        &function, taskName, i, &completed[i]
                    ));
            }
            cur += step;
        }
        // Wait unitl complete
        while(true) {
            bool allCompleted = true;
            for(int i = 0; i < threadCount; i++)
                if(!completed[i]) allCompleted = false;
            if(allCompleted) break;
            else std::this_thread::yield();
        }
        return true;
    }



    // Execute function on multi cores, may highly improve performance.
    template <class Function> bool multiThreadExecuteChannel(
        FMC& collection, 
        Function function, 
        std::string taskName = "$anonymous", 
        int threadCount = defaultThreadCount
    ) {

        // Check if able to chunk
        if(collection.width() < threadCount || collection.height() < threadCount) {
#ifdef LIBQIMG_SHOWLOG    
            printf("[Task \"%s\" #### ] Denied execution. Collection size too small.\n", 
                taskName.data());
#endif      
            return false;
        }
        pthread_t threads[threadCount];
        bool completed[threadCount];
        for(int i = 0; i < threadCount; i++)
            completed[i] = false;
        int cur = 0;
        int step = collection.height() / threadCount;
        // Create threads
        for(int i = 0; i < threadCount; i++) {
            if(i != threadCount - 1) {
                pthread_create(&threads[i], NULL, executeFMCTaskBlockChannel<Function>, 
                    new FMCTaskBlock<Function>(&collection,
                        Point(0, cur), Point(collection.width() - 1, cur + step - 1),
                        &function, taskName, i, &completed[i]
                    ));
            } else {
                pthread_create(&threads[i], NULL, executeFMCTaskBlockChannel<Function>, 
                    new FMCTaskBlock<Function>(&collection,
                        Point(0, cur), collection.end(),
                        &function, taskName, i, &completed[i]
                    ));
            }
            cur += step;
        }
        // Wait unitl complete
        while(true) {
            bool allCompleted = true;
            for(int i = 0; i < threadCount; i++)
                if(!completed[i]) allCompleted = false;
            if(allCompleted) break;
            else std::this_thread::yield();
        }
        return true;
    }


    // Execute function on multi cores, may highly improve performance.
    template <class Function> bool multiThreadExecute(
        FMAT& matrix, 
        Function function, 
        std::string taskName = "$anonymous", 
        int threadCount = defaultThreadCount
    ) {

        // Check if able to chunk
        if(matrix.width() < threadCount || matrix.height() < threadCount) {
#ifdef LIBQIMG_SHOWLOG    
            printf("[Task \"%s\" #### ] Denied execution. Matrix size too small.\n", 
                taskName.data());
#endif
            return false;
        }
        pthread_t threads[threadCount];
        bool completed[threadCount];
        for(int i = 0; i < threadCount; i++)
            completed[i] = false;
        int cur = 0;
        int step = matrix.height() / threadCount;
        // Create threads
        for(int i = 0; i < threadCount; i++) {
            if(i != threadCount - 1) {
                pthread_create(&threads[i], NULL, executeFMATTaskBlock<Function>, 
                    new FMATTaskBlock<Function>(&matrix,
                        Point(0, cur), Point(matrix.width() - 1, cur + step - 1),
                        &function, (char*)taskName.data(), i, &completed[i]
                    ));
            } else {
                pthread_create(&threads[i], NULL, executeFMATTaskBlock<Function>, 
                    new FMATTaskBlock<Function>(&matrix,
                        Point(0, cur), matrix.end(),
                        &function, (char*)taskName.data(), i, &completed[i]
                    ));
            }
            cur += step;
        }
        // Wait unitl complete
        while(true) {
            bool allCompleted = true;
            for(int i = 0; i < threadCount; i++)
                if(!completed[i]) allCompleted = false;
            if(allCompleted) break;
            else std::this_thread::yield();
        }
        return true;
    }

}

#endif