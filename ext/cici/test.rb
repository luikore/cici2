dir = File.dirname(__FILE__)
$:.unshift "#{dir}/../../lib"
$:.unshift "#{dir}/.."
require "cici"

Cici.app "hello", [400, 300] do |app|
  Cici::Internal.set_event_lock false
  app.on :size do |wp, lp|
    app.update
  end
end

