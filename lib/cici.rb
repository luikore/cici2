require "cici/support"
require "cici/internal"
require "cici/elem"

module Cici

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

  module_function

  # entry point
  def app *xs, &p
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

  # make color
  # rgb(0x12, 0x34, 0x56) == rgb(0x123456) == 0x563412
  def rgb r, g = nil, b = nil
    (g and b) ?
      ((b << 16) | (g << 8) | r) :
      (((r & 0xff) << 16) | (r & 0xff00) | ((r & 0xff0000) >> 16))
  end

end
