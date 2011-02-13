require "cici/ext"

module Cici

  # ease for creating child windows
  module Builder
    def elem *xs, &p
      Elem.new *xs, &p
    end
  end

  # element base
  class Elem
    include Builder

    # windows window_class string
    def window_class
      "canvas"
    end

    def initialize parent, title, size, &p
      Internal.new_child_window self, parent, title, 0, 0, *size
      p[self] if p
    end

    # TODO add mutex for event binding

    # bind callback to event, replaces previous event binding
    # form1:
    #   e.on :click, ->{ app.alert 'hello' }
    # form2:
    #   e.on :click do
    #     app.alert 'hello'
    #   end
    def on event, plambda=nil, &p
      p ||= plambda
      raise "require block or lambda, but got #{p.inspect}" unless p.is_a? Proc
      # TODO check if event exists
      define_singleton_method :"on_#{event}", p
    end

    # symetric to unbind
    alias bind on

    # remove callbacks binded on event
    def unbind event
      # TODO check if event exists
      instance_eval "undef on_#{event}"
    end
  end

end

