dir = File.dirname(__FILE__)
$:.unshift "#{dir}/../../lib"
$:.unshift "#{dir}/.."
require "cici/app"

class Object
  def toString
    self.to_s rescue self.inspect
  end
end

Cici.app "hello", [400, 300] do |app|
  Cici::Internal.set_event_lock false
  app.set_timer 5000 do
    raise 'e'
    Cici.alert 'hi'
  end
end

