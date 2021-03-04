module Seagull
    # Seagull::export(Sketchup.active_model.selection)
    def self.export(input)
        toWrite = createObject(input)

        json = "{\n"
        json = toJson(json, toWrite)
        json += "}\n"
        writeToDisk("C:/tmp/test.json", json) # TODO: Hardcoded place...

        json
    end

    def self.createObject(input_faces)
        faces = input_faces.select { |i| i.is_a?(Sketchup::Face) }

        output = {
            :version => 0,
            :data => {
                :ambient => 0.0,
                :camera => {
                    :position => [ 0, 3, 20 ],
                    :x =>        [ 1, 0, 0 ],
                    :y =>        [ 0, 1, 0 ],
                    :z =>        [ 0, 0, 1 ],
                },
                :materials => [
                    {
                        :emit_colour => [ 0.3, 0.4, 0.5 ],
                        :ref_colour => [ 0.0, 0.0, 0.0, 1.0 ],
                        :specular => 0.0,
                    },
                    {
                        :emit_colour => [ 80.0, 0.0, 0.0 ],
                        :ref_colour => [ 0.0, 0.0, 0.0, 1.0 ],
                        :specular => 0.0,
                    },
                    {
                        :emit_colour => [ 0.00, 0.00, 0.00 ],
                        :ref_colour => [ 0.5, 0.5, 0.5, 1.0 ],
                        :specular => 0.0,
                    }
                ],
                :faces => [],
            }
        }

        for face in faces
            outputFace = { :points => [], :material_index => 2 }
            for edge in face.edges
                vertice = edge.reversed_in?(face) ? edge.vertices[1] : edge.vertices[0]
                ptToAdd = [vertice.position.x.to_m.to_f, vertice.position.y.to_m.to_f, vertice.position.z.to_m.to_f]
                
                # TODO: To stop SketchUp output e.-1 nonsense...
                #ptToAdd[0] = (ptToAdd[0] < 0.0001) ? ptToAdd[0] : 0
                #ptToAdd[1] = (ptToAdd[1] < 0.0001) ? ptToAdd[1] : 0
                #ptToAdd[2] = (ptToAdd[2] < 0.0001) ? ptToAdd[2] : 0

                outputFace[:points] << ptToAdd
            end

            output[:data][:faces] << outputFace
        end

        output
    end

    def self.toJson(output, input)
        input.each do |key, value|
            #puts "#{output}\n\n-----\n\n\n"
            if value.is_a?(Hash)
                output += "#{key}: {\n"
                output = toJson(output, value)
                output += "},\n"
            elsif value.is_a?(Array)
                output += "#{key}: [\n"
                for v in value
                    if v.is_a?(Hash)
                        output += "{\n"
                        output = toJson(output, v)
                        output += "},\n"
                    elsif v.is_a?(Array)
                        output += "["
                        # TODO: Won't work for complex types of arrays-within-arrays.
                        for v2 in v
                            output += "#{v2}, "
                        end
                        output += "],\n"
                    else
                        output += "#{v}, "
                    end
                end
                output += "],\n"
            else
                output += "#{key}: #{value},\n"
            end
        end

        output
    end

    def self.writeToDisk(fname, data)
        File.open(fname, 'w') { |file| file.write(data) }
        nil
    end
end

pts = [[1.885, 2.44, 1.02], [0.0, 1.22000000000001, 1.02], [0.0, 0.0, 1.02], [3.77, 0.0, 1.02], [3.77, 4.59999999999999, 1.02]]