require "cici/internal"
require "cici/elem"

module Cici

  # entry point
  def Cici.app *xs, &p
    app = App.new *xs
    if p
      # for closure-block form, msg_loop is called right after the block invocation
      p[app]
      Internal.msg_loop
    else
      # for non-closure-block form, should call msg_loop manually
      App.send :class_eval, %Q[
        def msg_loop
          Internal.msg_loop
        end
      ]
    end
  end

  # top level element
  class App < Elem
    def initialize title, size
      if Internal.inited?
        raise 'can only init one instance'
      end
      Internal.new_top_window self, title, 0, 0, *size
    end

    def on_destroy wp, lp
      Internal.quit 0
    end
  end

end
