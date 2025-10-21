
// 1. Hello World GUI (SwiftUI)
// 2. Simple button with click -> alert
import SwiftUI

struct SUI01_HelloView: View {
    var body: some View {
        Text("Hello world!")
            .frame(maxWidth: .infinity, maxHeight: .infinity)
            .navigationTitle("Hello World program")
    }
}

struct SUI01_ButtonView: View {
    @State private var showAlert = false
    var body: some View {
        ZStack(alignment: .topLeading) {
            Button("Hello world!") { showAlert = true }
                .frame(width: 100, height: 30, alignment: .center)
                .padding(.leading, 30)
                .padding(.top, 30)
        }
        .navigationTitle("Button program")
        .alert("Popup message", isPresented: $showAlert) { Button("OK", role: .cancel) {} }
    }
}
