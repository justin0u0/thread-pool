package workerpool

import (
	"context"
	"log"
	"sync"
)

type Task struct {
	Func func(args ...interface{}) *Result
	Args []interface{}
}

type Result struct {
	Value interface{}
	Err   error
}

type WorkerPool interface {
	Start(ctx context.Context)
	Tasks() chan *Task
	Results() chan *Result
}

type workerPool struct {
	numWorkers int
	tasks      chan *Task
	results    chan *Result
	wg         *sync.WaitGroup
}

var _ WorkerPool = (*workerPool)(nil)

func NewWorkerPool(numWorkers int, bufferSize int) *workerPool {
	return &workerPool{
		numWorkers: numWorkers,
		tasks:      make(chan *Task, bufferSize),
		results:    make(chan *Result, bufferSize),
		wg:         &sync.WaitGroup{},
	}
}

func (wp *workerPool) Start(ctx context.Context) {
	wp.wg.Add(wp.numWorkers)

	for i := 0; i < wp.numWorkers; i++ {
		go wp.run(ctx)
	}

	wp.wg.Wait()
	close(wp.results)

	if err := ctx.Err(); err != nil {
		log.Println("gracefully shutdown all workers")
	} else {
		log.Println("all workers job done")
	}
}

func (wp *workerPool) Tasks() chan *Task {
	return wp.tasks
}

func (wp *workerPool) Results() chan *Result {
	return wp.results
}

func (wp *workerPool) run(ctx context.Context) {
	defer wp.wg.Done()

	for {
		select {
		case task, ok := <-wp.tasks:
			select {
			case <-ctx.Done():
				return
			default:
			}

			if !ok {
				return
			}

			result := task.Func(task.Args...)

			wp.results <- result
		case <-ctx.Done():
			return
		}
	}
}
