package workerpool

import (
	"context"
	"errors"
	"fmt"
	"testing"
	"time"

	. "github.com/onsi/ginkgo"
	. "github.com/onsi/gomega"
)

func TestWorkerPool(t *testing.T) {
	RegisterFailHandler(Fail)
	RunSpecs(t, "WorkerPool Suite")
}

func sleepPrint(args ...interface{}) *Result {
	if len(args) != 2 {
		return &Result{
			Err: errors.New("number of arguments mismatch"),
		}
	}

	d, ok := args[1].(time.Duration)
	if !ok {
		return &Result{
			Err: errors.New("expects args[1] to be type time.Duration"),
		}
	}

	value, ok := args[0].(int)
	if !ok {
		return &Result{
			Err: errors.New("expects args[0] to be type int"),
		}
	}

	time.Sleep(d)
	fmt.Println(value)

	return &Result{Value: value}
}

var _ = Describe("WorkerPool", func() {
	var wp *workerPool

	BeforeEach(func() {
		wp = NewWorkerPool(4, 1)
	})

	Describe("Start", func() {
		var ctx context.Context

		JustBeforeEach(func() {
			close(wp.Tasks())

			wp.Start(ctx)
		})

		When("done all tasks normally", func() {
			BeforeEach(func() {
				ctx = context.Background()

				wp.Tasks() <- &Task{Func: sleepPrint, Args: []interface{}{1, time.Millisecond}}
			})

			It("should receive results", func() {
				Expect(wp.Results()).To(Receive(Equal(&Result{
					Value: 1,
					Err:   nil,
				})))
			})
		})
	})
})
